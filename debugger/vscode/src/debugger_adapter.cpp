#include "debugger_adapter.h"
#include <string>
#include "kbase/at_exit_manager.h"
#include "ezio/io_service_context.h"
#include "ezio/event_loop.h"
#include "ezio/tcp_server.h"
#include "ezio/tcp_client.h"
#include "kbase/string_util.h"
#include "nlohmann\json.hpp"
#include <regex>
#include "debug_protocol.h"
#include "string_utils.h"
#include "script_system.h"
 #include "lua_net.h"
#include "net_thread_queue.h"

#define luaL_requirelib(L,name,fn) (luaL_requiref(L, name, fn, 1),lua_pop(L, 1))
extern "C" int luaopen_cjson(lua_State *L);

std::string LINES_ENDING = "";

using namespace ezio;

enum EDebugAdapterLaunchMode
{
	eLaunchMode_Launch = 0,
	eLaunchMode_Attach,
};

enum EDebugProtocolMode
{
	eMode_STDIO = 0,
	eMode_TCP,
};

std::string read_line_from_vscode()
{
	return "";
}

EDebugAdapterLaunchMode g_LaunchMode;
EDebugProtocolMode g_Mode;
void debugger_adapter_init(int argc, char* argv[])
{
	
}

String get_lua_path(const char* name)
{
	return string_format("%s/scripts/debugger/%s.lua", WORKSPACE_ROOT, name);
}

NetThreadQueue g_VscodeQueue;
NetThreadQueue g_RuntimeQueue;
TCPConnectionPtr g_VscodeHandler;
TCPConnectionPtr g_RuntimeHandler;
void check_lua_error(lua_State* L,int res)
{
	if (res != LUA_OK) { printf("%s\n", lua_tostring(L,-1)); }
}

void VscodeThreadFunc(int port)
{
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);
	luaL_requirelib(L, "cjson", luaopen_cjson);
	luaopen_netlib(L);
	luaopen_net_thread_queue(L);

	int res = luaL_loadfile(L, get_lua_path("common").c_str());
	check_lua_error(L, res);
	lua_pushstring(L, "vscode");
	res = lua_pcall(L, 1, LUA_MULTRET, 0);
	check_lua_error(L, res);

	EventLoop loop;
	SocketAddress addr(port);
	TCPServer server(&loop, addr, "VscodeServer");
	
	server.set_on_connection([L](const TCPConnectionPtr& conn) {
		g_VscodeHandler = conn->connected() ? conn : nullptr;
		if (g_VscodeHandler == nullptr)
		{
			g_VscodeQueue.Clear(NetThreadQueue::Read);
			g_VscodeQueue.Clear(NetThreadQueue::Write);
		}
		lua_getglobal(L, "vscode_on_connection");
		lua_push_tcp_connection(L, conn);
		int res = lua_pcall(L, 1, 0, 0);
		check_lua_error(L, res);
	});
	server.set_on_message([L](const TCPConnectionPtr& conn, Buffer& buf, TimePoint ts) {
		lua_getglobal(L, "vscode_on_message");
		lua_push_tcp_connection(L, conn);
		lua_push_ezio_buffer(L, buf);
		lua_push_net_thread_queue(L, &g_VscodeQueue);
		int res = lua_pcall(L, 3, 0, 0);
		check_lua_error(L, res);
	});
	server.Start();
	loop.RunTaskEvery([]() {
		if (g_VscodeHandler && g_VscodeHandler->connected())
		{
			while (!g_VscodeQueue.Empty(NetThreadQueue::Write))
			{
				auto msg = g_VscodeQueue.Front(NetThreadQueue::Write);
				g_VscodeQueue.PopFront(NetThreadQueue::Write);
				g_VscodeHandler->Send(msg);
			}
		}

	}, TimeDuration(1));

	loop.Run();
}

void RuntimeThreadFunc(const char* ip,int port)
{
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);
	luaL_requirelib(L, "cjson", luaopen_cjson);
	luaopen_netlib(L);
	luaopen_net_thread_queue(L);

	int res = luaL_loadfile(L, get_lua_path("common").c_str());
	check_lua_error(L, res);
	lua_pushstring(L, "runtime");
	res = lua_pcall(L, 1, LUA_MULTRET, 0);
	check_lua_error(L, res);

	SocketAddress addr(ip, port);
	EventLoop loop;
	TCPClient client(&loop, addr, "RuntimeClient");
	client.set_on_connection([L](const TCPConnectionPtr& conn) {
		g_RuntimeHandler = conn->connected() ? conn : nullptr;
		if (g_RuntimeHandler == nullptr)
		{
			g_RuntimeQueue.Clear(NetThreadQueue::Read);
			g_RuntimeQueue.Clear(NetThreadQueue::Write);
		}
		lua_getglobal(L, "runtime_on_connection");
		lua_push_tcp_connection(L, conn);
		int res = lua_pcall(L, 1, 0, 0);
		check_lua_error(L, res);
	});
	client.set_on_message([L](const TCPConnectionPtr& conn, Buffer& buf, TimePoint ts) {
		lua_getglobal(L, "runtime_on_message");
		lua_push_tcp_connection(L, conn);
		lua_push_ezio_buffer(L, buf);
		lua_push_net_thread_queue(L, &g_RuntimeQueue);
		int res = lua_pcall(L, 3, 0, 0);
		check_lua_error(L, res);
	});
	client.Connect();

	loop.RunTaskEvery([]() {
		if (g_RuntimeHandler && g_RuntimeHandler->connected())
		{
			while (!g_RuntimeQueue.Empty(NetThreadQueue::Write))
			{
				auto msg = g_RuntimeQueue.Front(NetThreadQueue::Write);
				g_RuntimeQueue.PopFront(NetThreadQueue::Write);
				g_RuntimeHandler->Send(msg);
			}
		}
	}, TimeDuration(1));

	loop.Run();
}

bool g_debugger_adapter_run = false;
std::vector<std::thread*> thread_set;

void vscode_on_launch_cmd(const char*cmd,const char* ip, int port)
{
	system(cmd);
	thread_set[1] = new std::thread(RuntimeThreadFunc, ip, port);
}

void vscode_on_attach_cmd(const char* ip, int port)
{
	if (thread_set[1] == nullptr)
	{
		thread_set[1] = new std::thread(RuntimeThreadFunc, ip, port);
	}
}

void set_debugger_adapter_run(bool run) { g_debugger_adapter_run = run; }

void set_line_ending_in_c(const char* le)
{
	LINES_ENDING = le;
}

int fetch_runtime_handler(lua_State* L)
{
	if (g_RuntimeHandler)
	{
		lua_push_tcp_connection(L, g_RuntimeHandler);
		return 1;
	}
	return 0;
}

int fetch_vscode_handler(lua_State* L)
{
	if (g_VscodeHandler != nullptr)
	{
		lua_push_tcp_connection(L, g_VscodeHandler);
		return 1;
	}
	return 0;
}

int debugger_adapter_run()
{
	ezio::IOServiceContext::Init();

	thread_set.resize(2);
	thread_set[0] = new std::thread(VscodeThreadFunc, 4711);

	lua_State* L = luaL_newstate();
	luaL_openlibs(L);
	luaL_requirelib(L, "cjson", luaopen_cjson);
	luaopen_netlib(L);
	luaopen_net_thread_queue(L);

	script_system_register_function(L, vscode_on_launch_cmd);
	script_system_register_function(L, vscode_on_attach_cmd);
	script_system_register_function(L, set_debugger_adapter_run);
	script_system_register_luac_function(L, fetch_runtime_handler);
	script_system_register_luac_function(L, fetch_vscode_handler);
	script_system_register_function(L, set_line_ending_in_c);

	int res = luaL_dofile(L, get_lua_path("main").c_str());
	check_lua_error(L, res);

	g_debugger_adapter_run = true;
	while (g_debugger_adapter_run)
	{
		while (!g_VscodeQueue.Empty(NetThreadQueue::Read))
		{
			auto msg = g_VscodeQueue.Front(NetThreadQueue::Read);
			g_VscodeQueue.PopFront(NetThreadQueue::Read);

			lua_getglobal(L, "dispatch_vscode_message");
			lua_push_net_thread_queue(L, &g_VscodeQueue);
			lua_pushstring(L, msg.c_str());
			lua_push_net_thread_queue(L, &g_RuntimeQueue);

			res = lua_pcall(L, 3, 0, 0);
			check_lua_error(L, res);
		}

		while (!g_RuntimeQueue.Empty(NetThreadQueue::Read))
		{
			auto msg = g_RuntimeQueue.Front(NetThreadQueue::Read);
			g_RuntimeQueue.PopFront(NetThreadQueue::Read);

			lua_getglobal(L, "dispatch_runtime_message");
			lua_push_net_thread_queue(L, &g_RuntimeQueue);
			lua_pushstring(L, msg.c_str());
			lua_push_net_thread_queue(L, &g_VscodeQueue);

			res = lua_pcall(L, 3, 0, 0);
			check_lua_error(L, res);
		}
	}

	for (auto* t : thread_set)
	{
		if (t->joinable())
		{
			t->join();
		}
		delete t;
	}
	thread_set.clear();

	return 0;
}