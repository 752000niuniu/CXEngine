#include "server.h"
#include "ezio/socket_address.h"
#include "ezio/tcp_server.h"
#include "tsv.h"
#include "protocol.h"
#include "ezio/endian_utils.h"
#include "ezio/buffer.h"
#include "actor/player.h"
#include "logger.h"
#include "script_system.h"
#include "net_thread_queue.h"
#include "lua_net.h"
#include "file_system.h"

std::map<uint64_t, TCPConnection*> g_PlayerConnections;
NetThreadQueue g_ReadPacketQueue;

struct ServerPacket
{
	Buffer* buff;
	TCPConnectionPtr conn;
};
std::thread* ServerThread;
GameServer* CXGameServer = NULL;


ezio::EventLoop g_MainLoop;

void net_send_message(uint64_t pid, int proto, const char* msg);

int net_send_message_to_players(lua_State* L);

int net_send_message_to_all_players(lua_State* L);

int insert_pid_connection_pair(lua_State* L);

int erase_pid_connection_pair(lua_State*L);
int netq_push_login_msg(lua_State*L);

#define luaL_requirelib(L,name,fn) (luaL_requiref(L, name, fn, 1),lua_pop(L, 1))
extern "C"  int luaopen_cjson(lua_State *L);

void thread_init_script_system(lua_State*L) {
#define REG_ENUM(name)  (lua_pushinteger(L, name),lua_setglobal(L, #name))
	REG_ENUM(PTO_C2S_SIGNUP);
	REG_ENUM(PTO_C2S_LOGIN);
	REG_ENUM(PTO_C2S_LOGOUT);
	REG_ENUM(PTO_C2S_MOVE_TO_POS);
	REG_ENUM(PTO_C2S_CHAT);
	REG_ENUM(PTO_S2C_PLAYER_ENTER);
	REG_ENUM(PTO_S2C_MOVE_TO_POS);
	REG_ENUM(PTO_S2C_CHAT);
	REG_ENUM(PTO_C2S_CONNECT);
	REG_ENUM(PTO_C2S_DISCONNECT);
#undef REG_ENUM
	(lua_pushinteger(L, CX_MSG_HEADER_LEN), lua_setglobal(L, "CX_MSG_HEADER_LEN"));
	script_system_register_function(L, net_send_message);
	script_system_register_luac_function(L, net_send_message_to_players);
	script_system_register_luac_function(L, net_send_message_to_all_players);
}

GameServer::GameServer(EventLoop* loop, SocketAddress addr, const char* name)
	:m_EventLoop(loop),
	m_Server(loop,addr,name)
{
	m_Server.set_on_connection(std::bind(&GameServer::OnConnection, this, std::placeholders::_1));
	m_Server.set_on_message(std::bind(&GameServer::OnMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	cxlog_info("server init at %s\n",addr.ToHostPort().c_str());
}

GameServer::~GameServer()
{

}

void GameServer::Start()
{
	m_L = luaL_newstate();
	luaL_openlibs(m_L);
	luaL_requirelib(m_L, "cjson", luaopen_cjson);
	luaopen_netlib(m_L);
	luaopen_net_thread_queue(m_L);
	luaopen_filesystem(m_L);

	thread_init_script_system(m_L);
	script_system_register_function(m_L, script_system_dofile);
	script_system_register_luac_function(m_L, netq_push_login_msg);
	script_system_register_luac_function(m_L, insert_pid_connection_pair);
	script_system_register_luac_function(m_L, erase_pid_connection_pair);


	char path[512];
	sprintf(path, "%sscripts/server/%s", FileSystem::GetPath().c_str(), "server.lua");
	if (luaL_dofile(m_L, path) != LUA_OK) {
		const char* msg = lua_tostring(m_L, -1);
		cxlog_err(msg);
		return;
	}

	script_system_call_function(m_L, "server_thread_start");
	m_Server.Start();
	cxlog_info("server start\n");
}

void GameServer::Stop()
{
	lua_close(m_L);
	cxlog_info("server stop\n");
	m_EventLoop->Quit();
}

void GameServer::SendMessageToPlayer(uint64_t pid, int proto, const char* msg)
{
	m_EventLoop->RunTask([this, pid, proto, msg]() {
		ezio::Buffer buf;
		buf.Write(proto);
		buf.Write(msg, strlen(msg));
		buf.Prepend((int)buf.readable_size());
		auto it = g_PlayerConnections.find(pid);
		if (it != g_PlayerConnections.end())
		{
			it->second->Send(kbase::StringView(buf.Peek(), buf.readable_size()));
		}
	});
}

void GameServer::SendMessageToPlayers(std::vector<uint64_t> pids, int proto, const char* msg)
{
	m_EventLoop->RunTask([this,  pids, proto, msg]() {
		ezio::Buffer buf;
		buf.Write(proto);
		buf.Write(msg, strlen(msg));
		buf.Prepend((int)buf.readable_size());
		for (auto pid : pids) {
			auto it = g_PlayerConnections.find(pid);
			if (it != g_PlayerConnections.end())
			{
				it->second->Send(kbase::StringView(buf.Peek(), buf.readable_size()));
			}
		}
	});
}

void GameServer::OnConnection(const TCPConnectionPtr& conn)
{
	const char* state = conn->connected() ? "connected" : "disconnected";
	cxlog_info("Connection %s is %s\n", conn->peer_addr().ToHostPort().c_str(), state);
	if (conn->connected())
	{
		for (auto& it : g_PlayerConnections) {
			if (it.second == conn.get()) {
				g_PlayerConnections.erase(it.first);
				break;
			}
		}
	}else {
	}
}

void GameServer::OnMessage(const TCPConnectionPtr& conn, Buffer& buf, TimePoint ts)
{
	lua_getglobal(m_L, "server_thread_on_message");
	lua_push_tcp_connection(m_L, conn);
	lua_push_ezio_buffer(m_L, buf);
	lua_push_net_thread_queue(m_L, &g_ReadPacketQueue);
	int res = lua_pcall(m_L, 3, 0, 0);
	check_lua_error(m_L, res);
}

void game_server_run(int port) {
	ezio::EventLoop loop;
	ezio::SocketAddress addr(port);
	CXGameServer = new GameServer(&loop, addr, "GameServer");
	CXGameServer->Start();
	loop.Run();
}

void game_server_start(int port) {
	ServerThread = new std::thread(game_server_run,  port);
}

int game_server_update(lua_State* L) {

	while (!g_ReadPacketQueue.Empty(NetThreadQueue::Read))
	{
		Buffer& pt = g_ReadPacketQueue.Front(NetThreadQueue::Read);
		lua_getglobal(L, "game_server_dispatch_message");
		lua_push_ezio_buffer(L, pt);
		int res = lua_pcall(L, 1, 0, 0);
		check_lua_error(L, res);
		g_ReadPacketQueue.PopFront(NetThreadQueue::Read);
	}
	return 0;
}

void game_server_stop() 
{
	CXGameServer->Stop();
	ServerThread->join();
}

void net_send_message(uint64_t pid, int proto, const char* msg) {
	cxlog_info("net_send_message pid:%lld proto:%d js:%s\n", pid, proto, msg);
	CXGameServer->SendMessageToPlayer(pid, proto, msg);
}

int net_send_message_to_players(lua_State* L) {
	std::vector<uint64_t> pids;
	int len = (int)luaL_len(L, 1);
	int i = 0;
	lua_pushnil(L);
	while (lua_next(L, 1) != 0) {
		uint64_t pid = (uint64_t)lua_tointeger(L, -1);
		pids.push_back(pid);
		i++;
		lua_pop(L, 1);
	}
	int proto = (int)lua_tointeger(L, 2);
	const char* msg = lua_tostring(L, 3);
	cxlog_info("net_send_message_to_players proto:%d js:%s\n",  proto, msg);
	CXGameServer->SendMessageToPlayers(pids, proto, msg);
	return 0;
}

int net_send_message_to_all_players(lua_State* L) {
	int proto = (int)lua_tointeger(L, 1);
	const char* msg = lua_tostring(L, 2);
	ezio::Buffer buf;
	buf.Write(proto);
	buf.Write(msg, strlen(msg));
	buf.Prepend((int)buf.readable_size());
	cxlog_info("net_send_message_to_all_players proto:%d js:%s\n", proto, msg);
	CXGameServer->GetLoop()->RunTask([buf]() {
		for (auto& it : g_PlayerConnections) {
			it.second->Send({ buf.Peek(),buf.readable_size() });
		}
	}); 
	return 0;
}

int insert_pid_connection_pair(lua_State* L){
	uint64_t pid = (uint64_t)lua_tointeger(L, 1);
	TCPConnection* conn = lua_check_tcpconnection(L, 2);
	if (g_PlayerConnections.find(pid) != g_PlayerConnections.end()) {
		return 0;
	}
	
	g_PlayerConnections.insert({ pid,conn });
	return 0;
}

int erase_pid_connection_pair(lua_State*L ) {
	uint64_t pid = (uint64_t)lua_tointeger(L, 1);
	g_PlayerConnections.erase(pid);
	return 0;
}

int netq_push_login_msg(lua_State*L) {
	NetThreadQueue* q = lua_check_net_thread_queue(L, 1);
	const char* msg = lua_tostring(L, 2);
	Buffer buf;
	buf.Write((int)PTO_C2S_LOGIN);
	buf.Write(msg, strlen(msg));
	q->PushBack(NetThreadQueue::Read, buf.Peek(), buf.readable_size());
	return 0;
}



void luaopen_game_server(lua_State* L)
{
	thread_init_script_system(L);

	script_system_register_function(L, game_server_start);
	script_system_register_luac_function(L, game_server_update);
	script_system_register_function(L, game_server_stop);
}

void main_game_update() {
	if (!script_system_update()) {
		g_MainLoop.Quit();
	}
}

void game_main_run()
{
	script_system_init();
	g_MainLoop.RunTaskEvery(main_game_update, TimeDuration(16));
	g_MainLoop.Run();
	script_system_deinit();
}