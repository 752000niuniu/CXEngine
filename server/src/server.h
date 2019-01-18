#pragma once
#include "Singleton.h"
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
private:
	void SendS2CPlayerEnter(const TCPConnectionPtr& conn, Player* player,bool is_local);

	void SendS2CPlayer(const TCPConnectionPtr& conn, char* data,int size);

	void OnFrameUpdate();
	void OnConnection(const TCPConnectionPtr& conn);
	void OnMessage(const TCPConnectionPtr& conn, Buffer& buf, TimePoint ts);
	int m_Port;
	EventLoop* m_EventLoop;
	TCPServer m_Server;
	typedef std::set<TCPConnectionPtr> ConnectionList;
	ConnectionList m_Connections;

};

