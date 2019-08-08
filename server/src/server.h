#pragma once
#include "singleton.h"
#include "ezio/event_loop.h"
#include "kbase/basic_macros.h"
#include "ezio/socket_address.h"
#include "ezio/tcp_connection.h"
#include "ezio/tcp_server.h"
#include "actor/player.h"
using namespace ezio;



class GameServer 
{
public:
	GameServer(EventLoop* loop, SocketAddress addr,const char* name);
	~GameServer();

	DISALLOW_COPY(GameServer);
	DISALLOW_MOVE(GameServer);

	void Start();
	void Stop();
	
	void SendS2CPlayerEnter(const TCPConnectionPtr& conn, Player* player, bool is_local);
	void SendS2CPlayer(const TCPConnectionPtr& conn, char* data, int size);

	void SendMessageToPlayer(int pid, int proto, const char* msg);
	void SendMessageToPlayers(std::vector<uint64_t> pids, int proto, const char* msg);

	EventLoop* GetLoop() { return m_EventLoop; }
private:
	

	void OnConnection(const TCPConnectionPtr& conn);
	void OnMessage(const TCPConnectionPtr& conn, Buffer& buf, TimePoint ts);
	int m_Port;
	EventLoop* m_EventLoop;
	TCPServer m_Server;
	lua_State* m_L;
};

void game_main_run();

void game_server_start(int port);
void game_server_stop();

void luaopen_game_server(lua_State* L);