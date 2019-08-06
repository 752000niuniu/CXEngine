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

std::map<uint64_t, TCPConnectionPtr> g_PlayerConnections;
NetThreadQueue g_ReadPacketQueue;

struct ServerPacket
{
	Buffer* buff;
	TCPConnectionPtr conn;
};

std::mutex g_MsgQLock;

std::thread* ServerThread;
GameServer* CXGameServer = NULL;


ezio::EventLoop g_MainLoop;


#define luaL_requirelib(L,name,fn) (luaL_requiref(L, name, fn, 1),lua_pop(L, 1))
extern "C"  int luaopen_cjson(lua_State *L);


void check_lua_error(lua_State* L, int res)
{
	if (res != LUA_OK) { printf("%s\n", lua_tostring(L, -1)); }
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

	char path[512];
	sprintf(path, "%s/scripts/server/%s", WORK_DIR, "server.lua");
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

void GameServer::SendS2CPlayerEnter(const TCPConnectionPtr& conn, Player* player, bool is_local)
{
	Buffer buf;
	buf.Write((int)PTO_S2C_PLAYER_ENTER);
	buf.Write((int)player->GetNickName().size());
	buf.Write(player->GetNickName().data(), player->GetNickName().size());
	buf.Write(player->GetSceneID());
	buf.Write(player->GetDir());
	buf.Write(player->GetRoleID());
	buf.Write(player->GetWeaponID());
	buf.Write(player->GetPos().x);
	buf.Write(player->GetPos().y);
	buf.Write((int)(is_local ? 1 : 0));
	buf.Prepend((int)buf.readable_size());
	conn->Send(kbase::StringView(buf.Peek(), buf.readable_size()));
	buf.ConsumeAll();
}

void GameServer::SendS2CPlayer(const TCPConnectionPtr& conn, char* data, int size)
{
	conn->Send(kbase::StringView(data, size));
}



void GameServer::SendMessageToPlayer(int pid, int proto, const char* msg)
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
	m_EventLoop->RunTask([this, pids, proto, msg]() {
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
		m_Connections.insert(conn);
	}
	else
	{
		for (auto& it : g_PlayerConnections) {
			if (it.second == conn) {
				g_PlayerConnections.erase(it.first);
				break;
			}
		}
		m_Connections.erase(conn);
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


	while (buf.readable_size() >= CX_MSG_HEADER_LEN)
	{
		int len = buf.PeekAsInt32();
		if (buf.readable_size() >= len + CX_MSG_HEADER_LEN)
		{
			buf.Consume(CX_MSG_HEADER_LEN);
			int type = buf.PeekAsInt32();
			if (type == PTO_C2S_CONNECT) {
				buf.Consume(4);
				int pid = buf.ReadAsInt32();
				g_PlayerConnections.insert({ pid,conn });
				return;
			}
			else if (type == PTO_C2S_DISCONNECT) {
				buf.Consume(4);
				int pid = buf.ReadAsInt32();
				g_PlayerConnections.erase(pid);
			}
			else {
				g_ReadPacketQueue.PushBack(NetThreadQueue::Read, buf.ReadAsString(len));
			}
		}
		else
		{
			break;
		}
	}
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
		std::string& msg = g_ReadPacketQueue.Front(NetThreadQueue::Read);
		Buffer*pt = new Buffer();
		pt->Write(msg.data(), msg.size());
		g_ReadPacketQueue.PopFront(NetThreadQueue::Read);

		lua_getglobal(L, "game_server_dispatch_message");
		lua_push_ezio_buffer(L, *pt);
		int res = lua_pcall(L, 1, 0, 0);
		delete pt;

		int type = pt->ReadAsInt32();
		switch (type)
		{
		case PTO_C2S_LOGIN:
		{
			std::string name;
			int namelen = pt->ReadAsInt32();
			name = pt->ReadAsString(namelen);
			int scene_id = pt->ReadAsInt32();
			int dir = pt->ReadAsInt32();
			int role_id = pt->ReadAsInt32();
			int weapon_id = pt->ReadAsInt32();
			float pos_x = pt->ReadAsFloat();
			float pos_y = pt->ReadAsFloat();

			auto old = actor_manager_find_player_by_name(name.c_str());
			if (old) {
				actor_manager_remove_player_by_name(name.c_str());
			}
			auto player = actor_manager_create_player(role_id, name.c_str());
			player->SetSceneID(scene_id);
			player->SetWeaponID(weapon_id);
			player->SetDir(dir);
			player->SetPos(pos_x, pos_y);
			/*
				A玩家登录,其他玩家要知道
				A玩家登录,要同步其他玩家信息
			*/
			for (auto& it : g_PlayerConnections) {
				if (it.first != player->GetID()) {
					CXGameServer->SendS2CPlayer(it.second, (char*)pt->Peek(), pt->readable_size());
				}
				else {
					auto allplayers = actor_manager_fetch_all_players();
					for (size_t i = 0; i < allplayers.size(); i++)
					{
						auto* player_i = allplayers[i];
						int pid = player_i->GetID();
						if (pid != it.first) {
							CXGameServer->SendS2CPlayer(g_PlayerConnections[player->GetID()], (char*)pt->Peek(), pt->readable_size());
						}
					}
				}
			}
		}
		break;
		case PTO_C2S_LOGOUT:
		{

		}
		break;
		case PTO_C2S_MOVE_TO_POS:
		{
			std::string name;
			int namelen = pt->ReadAsInt32();
			name = pt->ReadAsString(namelen);

			pt->Prepend(name.data(), name.size());
			pt->Prepend(namelen);
			pt->Prepend((int)PTO_S2C_MOVE_TO_POS);
			pt->Prepend((int)pt->readable_size());

			auto player = actor_manager_find_player_by_name(name.c_str());
			auto allplayers = actor_manager_fetch_all_players();
			for (auto& it : g_PlayerConnections) {
				if (it.first != player->GetID()) {
					CXGameServer->SendS2CPlayer(it.second, (char*)pt->Peek(), pt->readable_size());
				}
			}
		}
		break;
		case PTO_C2S_CHAT:
		{
			std::string name;
			int namelen = pt->ReadAsInt32();
			name = pt->ReadAsString(namelen);

			pt->Prepend(name.data(), name.size());
			pt->Prepend(namelen);
			pt->Prepend((int)PTO_S2C_CHAT);
			pt->Prepend((int)pt->readable_size());

			auto player = actor_manager_find_player_by_name(name.c_str());
			auto allplayers = actor_manager_fetch_all_players();
			for (auto& it : g_PlayerConnections) {
				if (it.first != player->GetID()) {
					CXGameServer->SendS2CPlayer(it.second, (char*)pt->Peek(), pt->readable_size());
				}
			}
		}
		break;
		default:
			break;
		}

	}
}

void game_server_stop() 
{
	CXGameServer->Stop();
	ServerThread->join();
}

void net_send_message(int pid, int proto, const char* msg) {
	
	CXGameServer->SendMessageToPlayer(pid, proto, msg);
	
}

int net_send_message_to_all(lua_State* L) {
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
	int proto = lua_tointeger(L, 2);
	const char* msg = lua_tostring(L, 3);
	CXGameServer->SendMessageToPlayers(pids, proto, msg);
	return 0;
}

int insert_pid_connection_pair(lua_State* L){
	uint64_t pid = (uint64_t)lua_tointeger(L, 1);
	TCPConnection* conn = lua_check_tcpconnection(L, 2);
	g_PlayerConnections.insert({ pid,TCPConnectionPtr(conn) });
	return 0;
}

int erase_pid_connection_pair(lua_State*L ) {
	uint64_t pid = (uint64_t)lua_tointeger(L, 1);
	g_PlayerConnections.erase(pid);
	return 0;
}


void luaopen_game_server(lua_State* L)
{
#define REG_ENUM(name)  (lua_pushinteger(L, name),lua_setglobal(L, #name))
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

	script_system_register_function(L, game_server_start);
	script_system_register_luac_function(L, game_server_update);
	script_system_register_function(L, game_server_stop);

	script_system_register_function(L, net_send_message);
	script_system_register_luac_function(L, net_send_message_to_all);

	script_system_register_function(L, insert_pid_connection_pair);
	script_system_register_function(L, erase_pid_connection_pair);

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