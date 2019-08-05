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

std::map<int, TCPConnectionPtr> m_PlayerConnections;

struct ServerPacket
{
	Buffer* buff;
	TCPConnectionPtr conn;
};

std::mutex g_MsgQLock;
std::deque<Buffer*> g_ReadPacketQueue;

std::thread* ServerThread;
GameServer* CXGameServer = NULL;

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
	m_Server.Start();
	cxlog_info("server start\n");
}

void GameServer::Stop()
{
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
		for (auto& it : m_PlayerConnections) {
			if (it.second == conn) {
				m_PlayerConnections.erase(it.first);
				break;
			}
		}
		m_Connections.erase(conn);
	}
}

void GameServer::OnMessage(const TCPConnectionPtr& conn, Buffer& buf, TimePoint ts)
{
	while (buf.readable_size() >= CX_MSG_HEADER_LEN)
	{
		int len = buf.PeekAsInt32();
		if (buf.readable_size() >= len + CX_MSG_HEADER_LEN)
		{
			buf.Consume(CX_MSG_HEADER_LEN);

			Buffer* pt = new Buffer(len);
			pt->Write(buf.Peek(), len);
			buf.Consume(len);

			int type = pt->ReadAsInt32();
			switch (type)
			{
			case PTO_C2S_LOGIN:
				int pid = pt->ReadAsInt32();
				m_PlayerConnections.insert({ pid,conn });
				break;
			case PTO_C2S_LOGOUT:
				int pid = pt->ReadAsInt32();
				m_PlayerConnections.erase(pid);
				break;
			}
			
			g_MsgQLock.lock();
			g_ReadPacketQueue.push_back(pt);
			g_MsgQLock.unlock();
		}
		else
		{
			break;
		}
	}

	while (buf.readable_size() >= CX_MSG_HEADER_LEN)
	{
		int len = buf.PeekAsInt32();
		if (buf.readable_size() >= len + CX_MSG_HEADER_LEN)
		{
			buf.Consume(CX_MSG_HEADER_LEN);
			Buffer pt(len);
			pt.Write((char*)buf.Peek(), len);

			int type = pt.ReadAsInt32();
			switch (type)
			{
			case PTO_C2S_LOGIN:
			{
				std::string name;
				int namelen = pt.ReadAsInt32();
				name = pt.ReadAsString(namelen);
				int scene_id = pt.ReadAsInt32();
				int dir = pt.ReadAsInt32();
				int role_id = pt.ReadAsInt32();
				int weapon_id = pt.ReadAsInt32();
				float pos_x = pt.ReadAsFloat();
				float pos_y = pt.ReadAsFloat();

				auto old = actor_manager_find_player_by_name(name.c_str());
				if (old)
				{
					actor_manager_remove_player_by_name(name.c_str());
				}
				auto player = actor_manager_create_player(role_id, name.c_str());
				player->SetSceneID(scene_id);
				player->SetWeaponID(weapon_id);
				player->SetDir(dir);
				player->SetPos(pos_x, pos_y);

				auto allplayers = actor_manager_fetch_all_players();
				for (size_t i = 0; i < allplayers.size(); i++)
				{
					SendS2CPlayerEnter(conn, allplayers[i], player->GetNickName() == allplayers[i]->GetNickName());
				}

				for (auto& playerconn : m_Connections)
				{
					if (playerconn->peer_addr().ToHostPort() != conn->peer_addr().ToHostPort())
					{
						SendS2CPlayerEnter(playerconn, player, false);
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
				int namelen = pt.ReadAsInt32();
				name = pt.ReadAsString(namelen);

				pt.Prepend(name.data(), name.size());
				pt.Prepend(namelen);
				pt.Prepend((int)PTO_S2C_MOVE_TO_POS);
				pt.Prepend((int)pt.readable_size());

				auto player = actor_manager_find_player_by_name(name.c_str());
				auto allplayers = actor_manager_fetch_all_players();
				for (auto& playerconn : m_Connections)
				{
					if (playerconn->peer_addr().ToHostPort() != conn->peer_addr().ToHostPort())
					{
						SendS2CPlayer(playerconn, (char*)pt.Peek(), pt.readable_size());
					}
				}
			}
			break;
			case PTO_C2S_CHAT:
			{
				std::string name;
				int namelen = pt.ReadAsInt32();
				name = pt.ReadAsString(namelen);

				pt.Prepend(name.data(), name.size());
				pt.Prepend(namelen);
				pt.Prepend((int)PTO_S2C_CHAT);
				pt.Prepend((int)pt.readable_size());

				auto player = actor_manager_find_player_by_name(name.c_str());
				auto allplayers = actor_manager_fetch_all_players();

				for (auto& playerconn : m_Connections)
				{
					if (playerconn->peer_addr().ToHostPort() != conn->peer_addr().ToHostPort())
					{
						SendS2CPlayer(playerconn, (char*)pt.Peek(), pt.readable_size());
					}
				}
			}
			break;
			default:
				break;
			}
			buf.Consume(len);

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

void game_server_update() {
	while (!g_ReadPacketQueue.empty())
	{

		Buffer* pt = g_ReadPacketQueue.front();
		g_MsgQLock.lock();
		g_ReadPacketQueue.pop_front();
		g_MsgQLock.unlock();

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
			if (old)
			{
				actor_manager_remove_player_by_name(name.c_str());
			}
			auto player = actor_manager_create_player(role_id, name.c_str());
			player->SetSceneID(scene_id);
			player->SetWeaponID(weapon_id);
			player->SetDir(dir);
			player->SetPos(pos_x, pos_y);

			CXGameServer->SendS2CPlayerEnterAll();
			auto allplayers = actor_manager_fetch_all_players();
			for (size_t i = 0; i < allplayers.size(); i++)
			{
				CXGameServer->SendS2CPlayerEnter(conn, allplayers[i], player->GetNickName() == allplayers[i]->GetNickName());
			}

			for (auto& playerconn : m_Connections)
			{
				if (playerconn->peer_addr().ToHostPort() != conn->peer_addr().ToHostPort())
				{
					SendS2CPlayerEnter(playerconn, player, false);
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
			for (auto& playerconn : m_Connections)
			{
				if (playerconn->peer_addr().ToHostPort() != conn->peer_addr().ToHostPort())
				{
					SendS2CPlayer(playerconn, (char*)pt->Peek(), pt->readable_size());
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

			for (auto& playerconn : m_Connections)
			{
				if (playerconn->peer_addr().ToHostPort() != conn->peer_addr().ToHostPort())
				{
					SendS2CPlayer(playerconn, (char*)pt->Peek(), pt->readable_size());
				}
			}
		}
		break;
		default:
			break;
		}

		delete pt;
	}
}

void game_server_stop() 
{
	CXGameServer->Stop();
	ServerThread->join();
}

void luaopen_game_server(lua_State* L)
{
	script_system_register_function(L, game_server_start);
	script_system_register_function(L, game_server_update);
	script_system_register_function(L, game_server_stop);
}
