#include "debugger.h"
#include <functional>

#include "kbase/at_exit_manager.h"
#include "ezio/io_context.h"
#include "ezio/io_service_context.h"
#include "ezio/event_loop.h"
#include "ezio/socket_address.h"
#include "ezio/tcp_server.h"
#include "ezio/tcp_connection.h"
#include "ezio/buffer.h"
#include "ezio/acceptor.h"
#include "ezio/connector.h"


#include "script_system.h"
#include "debug_protocol.h"
#include "net_thread_queue.h"
#include "lua_net.h"
#include "file_system.h"

using namespace ezio;

std::string LINES_ENDING = "";
void set_line_ending_in_c(const char* le)
{
	LINES_ENDING = le;
}
const char* get_line_ending_in_c()
{
	return LINES_ENDING.c_str();
}

void debugger_update_session();

std::thread* debuggee_thread;
NetThreadQueue g_DebugAdapterQueue;
TCPConnectionPtr g_DebugAdapterHandler;
TimerID g_RuntimeSendTimerID ;
EventLoop* g_RuntimeServerLoop = nullptr;

void check_lua_error(lua_State* L, int res)
{
	if (res != LUA_OK) { printf("%s\n", lua_tostring(L, -1)); }
}

#define luaL_requirelib(L,name,fn) (luaL_requiref(L, name, fn, 1),lua_pop(L, 1))
extern "C" int luaopen_cjson(lua_State *L);
void DebuggeeThreadFunc(int port)
{
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);
	luaL_requirelib(L, "cjson", luaopen_cjson);
	luaopen_netlib(L);
	luaopen_net_thread_queue(L);

	script_system_register_function(L, set_line_ending_in_c);

	int res = luaL_loadfile(L, FileSystem::GetLuaPath("debuggee.lua").c_str());
	check_lua_error(L, res);
	lua_pushstring(L, "debuggee");
	res = lua_pcall(L, 1, LUA_MULTRET, 0);
	check_lua_error(L, res);

	EventLoop loop;
	g_RuntimeServerLoop = &loop;
	SocketAddress addr(port);
	TCPServer server(&loop, addr, "RuntimeServer");

	server.set_on_connection([L](const TCPConnectionPtr& conn) {
		g_DebugAdapterHandler = conn->connected() ? conn : nullptr;
		if (g_DebugAdapterHandler) 
		{
			g_RuntimeSendTimerID = g_RuntimeServerLoop->RunTaskEvery([]() {
				if (g_DebugAdapterHandler && g_DebugAdapterHandler->connected())
				{
					while (!g_DebugAdapterQueue.Empty(NetThreadQueue::Write))
					{
						auto msg = g_DebugAdapterQueue.Front(NetThreadQueue::Write);
						g_DebugAdapterQueue.PopFront(NetThreadQueue::Write);
						g_DebugAdapterHandler->Send(msg);
					}
				}
			}, TimeDuration(1));
		}
		else
		{
			if (g_RuntimeSendTimerID)
			{
				g_RuntimeServerLoop->CancelTimedTask(g_RuntimeSendTimerID);
			}
		}

		lua_getglobal(L, "debuggee_on_connection");
		lua_push_tcp_connection(L, conn);
		lua_push_net_thread_queue(L, &g_DebugAdapterQueue);
		int res = lua_pcall(L, 2, 0, 0);
		check_lua_error(L, res);
	});
	server.set_on_message([L](const TCPConnectionPtr& conn, Buffer& buf, TimePoint ts) {
		lua_getglobal(L, "debuggee_on_message");
		lua_push_tcp_connection(L, conn);
		lua_push_ezio_buffer(L, buf);
		lua_push_net_thread_queue(L, &g_DebugAdapterQueue);
		int res = lua_pcall(L, 3, 0, 0);
		check_lua_error(L, res);
	});
	server.Start();

	
	loop.Run();
}

void debugger_start_session()
{
	auto portstr = script_system_get_config("--debug_port");
	if (strcmp(portstr, "") != 0)
	{
		int port = std::stoi(portstr);
		debuggee_thread = new std::thread(DebuggeeThreadFunc, port);
	}
}

void debugger_stop_session()
{
	if(g_RuntimeServerLoop)
		g_RuntimeServerLoop->Quit();

	if (debuggee_thread)
	{
		debuggee_thread->join();
		delete debuggee_thread;
	}
}

void debugger_update_session()
{
	lua_State* L = script_system_get_luastate();
	lua_getglobal(L, "lua_debugger_update_session");
	lua_push_net_thread_queue(L, &g_DebugAdapterQueue);
	int res = lua_pcall(L, 1, 0, 0);
	check_lua_error(L, res);
}

void debugger_sleep(int s)
{
	Sleep(s);
}

const char* debugger_fetch_message()
{
	if (!g_DebugAdapterQueue.Empty(NetThreadQueue::Read))
	{
		static std::string msg;
		msg = g_DebugAdapterQueue.Front(NetThreadQueue::Read);
		g_DebugAdapterQueue.PopFront(NetThreadQueue::Read);
		return msg.c_str();
	}
	return "";
}

void debugger_send_message(const char* msg)
{
	g_DebugAdapterQueue.PushBack(NetThreadQueue::Write,msg);
}

bool debugger_is_connected()
{
	return g_DebugAdapterHandler != nullptr && g_DebugAdapterHandler->connected();
}


void luaopen_debugger(lua_State* L)
{
	script_system_register_function(L, debugger_start_session);
	script_system_register_function(L, debugger_update_session);
	script_system_register_function(L, debugger_stop_session);
	script_system_register_function(L, debugger_sleep);

	script_system_register_function(L, debugger_fetch_message);
	script_system_register_function(L, debugger_send_message);
	script_system_register_function(L, debugger_is_connected);
	script_system_register_function(L, get_line_ending_in_c);
	
}

