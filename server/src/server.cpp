#include "server.h"
#include "ezio/socket_address.h"
#include "ezio/tcp_server.h"
#include "tsv.h"
#include "protocol.h"
#include "ezio/endian_utils.h"
#include "ezio/buffer.h"

GameServer::GameServer(EventLoop* loop, SocketAddress addr, const char* name)
	:m_EventLoop(loop),
	m_Server(loop,addr,name)
{
	m_Server.set_on_connection(std::bind(&GameServer::OnConnection, this, std::placeholders::_1));
	m_Server.set_on_message(std::bind(&GameServer::OnMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	printf("server init at %s\n",addr.ToHostPort().c_str());
}

GameServer::~GameServer()
{

}

void GameServer::Start()
{
	m_EventLoop->RunTaskEvery(std::bind(&GameServer::OnFrameUpdate, this), TimeDuration(16));
	m_Server.Start();
	printf("server start\n");
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

void GameServer::OnFrameUpdate()
{

}

void GameServer::OnConnection(const TCPConnectionPtr& conn)
{
	const char* state = conn->connected() ? "connected" : "disconnected";
	printf("Connection %s is %s\n", conn->peer_addr().ToHostPort().c_str(), state);
	if (conn->connected())
	{
		m_Connections.insert(conn);
	}
	else
	{
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

				pt.Prepend(name.data(),name.size());
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

