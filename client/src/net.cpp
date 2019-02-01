#include "net.h"

#include <stdio.h>
#include "script_system.h"
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
#include "ezio/tcp_client.h"

#include "scene/BaseScene.h"
#include "kbase/pickle.h"
#include "scene/SceneManager.h"

using namespace ezio;

std::mutex g_MsgQLock;
std::deque<Buffer*> g_ReadPacketQueue;

class NetClient
{
public:
	NetClient(EventLoop* loop, const SocketAddress& serverAddr, const char* name);

	~NetClient();
	void Connect();
	void SendMoveToPosMsg(MSGC2SMoveToPos* msg);

	void SendMSGC2SLogin(MSGC2SLogin* msg);

	void SendMSGC2SLogout(MSGC2SLogout* msg);

	void SendMSGC2SChat(MSGC2SChat * msg);

	void SendMessageToGame(char* buf, size_t len);

	void Disconnect() { m_Client.Disconnect(); };

private:
	void OnConnection(const TCPConnectionPtr&);

	void OnMessage(const TCPConnectionPtr&, Buffer&, TimePoint);

	TCPClient m_Client;
};


NetClient::NetClient(EventLoop* loop, const SocketAddress& serverAddr,const char* name)
	:m_Client(loop, serverAddr, name)
{
	m_Client.set_on_connection(std::bind(&NetClient::OnConnection, this, std::placeholders::_1));
	m_Client.set_on_message(std::bind(&NetClient::OnMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

NetClient::~NetClient()
{

}

void NetClient::Connect()
{
	m_Client.Connect();
}

void NetClient::SendMoveToPosMsg(MSGC2SMoveToPos* msg)
{
	
	m_Client.connection()->Send(kbase::StringView((char*)msg, msg->len));
}

void NetClient::SendMSGC2SLogin(MSGC2SLogin* msg)
{
	ezio::Buffer buf;
	buf.Write(msg->type);
	buf.Write(msg->namelen);
	buf.Write(msg->name.data(), msg->namelen);
	buf.Write(msg->scene_id);
	buf.Write(msg->dir);
	buf.Write(msg->role_id);
	buf.Write(msg->weapon_id);
	buf.Write(msg->pos_x);
	buf.Write(msg->pos_y);
	int cnt = (int)buf.readable_size();
	buf.Prepend(cnt);
	auto sv = kbase::StringView(buf.Peek(), buf.readable_size());
	auto str = sv.ToString();
	m_Client.connection()->Send(sv);
}

void NetClient::SendMSGC2SLogout(MSGC2SLogout* msg)
{
	m_Client.connection()->Send(kbase::StringView((char*)msg, msg->len));
}

void NetClient::SendMSGC2SChat(MSGC2SChat * msg)
{
	if (m_Client.connection()->connected())
	{
		m_Client.connection()->Send(kbase::StringView((char*)msg, msg->len));
	}
}

void NetClient::SendMessageToGame(char* buf, size_t len)
{
	if (m_Client.connection() != nullptr && m_Client.connection()->connected())
	{
		m_Client.connection()->Send(kbase::StringView(buf, len));
	}
}

void NetClient::OnConnection(const TCPConnectionPtr& conn)
{
	const char* state = conn->connected() ? "connected" : "disconnected";
	printf("Connection %s is %s\n", conn->peer_addr().ToHostPort().c_str(), state);

}

void NetClient::OnMessage(const TCPConnectionPtr& conn, Buffer& buf, TimePoint time)
{
	while (buf.readable_size() >= CX_MSG_HEADER_LEN)
	{
		int len = buf.PeekAsInt32();
		if (buf.readable_size() >= len + CX_MSG_HEADER_LEN)
		{
			buf.Consume(CX_MSG_HEADER_LEN);
			
			Buffer* msg = new Buffer(len);
			msg->Write(buf.Peek(), len);
			buf.Consume(len);

			g_MsgQLock.lock();
			g_ReadPacketQueue.push_back(msg);
			g_MsgQLock.unlock();
		}
		else
		{
			break;
		}
	}
}

NetClient* g_Client = nullptr;
EventLoop* g_Loop = nullptr;

class NetThread : public Singleton<NetThread>
{
public:
	NetThread();
	~NetThread();
	void Init();
	void Update();
	void Deinit();
	void Run(std::string ip,int port);
private:
	std::thread* m_Thread;
};

NetThread::NetThread()
{
	
}

NetThread::~NetThread()
{
	
}

// ÍøÂçÏß³Ìº¯Êý
void NetThread::Run(std::string ip,int port)
{
	EventLoop loop;
	g_Loop = &loop;
	SocketAddress addr(ip.c_str(), port);
	NetClient client(&loop,addr,"Client");
	g_Client = &client;
	client.Connect();
	loop.Run();
}

void NetThread::Init()
{
	std::string ip;
	int port = 45000;
	ip = script_system_get_config("--server-ip");
	if (ip == "")
	{
		ip = "127.0.0.1";
	}
	const char* portstr = script_system_get_config("--server-port");
	if (strcmp(portstr, "") != 0)
	{
		port = std::stoi(portstr);
	}
	m_Thread = new std::thread(std::bind(&NetThread::Run, this, std::placeholders::_1, std::placeholders::_2), ip, port);
}

void NetThread::Update()
{
	while (!g_ReadPacketQueue.empty())
	{
		Buffer* pt = g_ReadPacketQueue.front();
		g_MsgQLock.lock();
		g_ReadPacketQueue.pop_front();
		g_MsgQLock.unlock();

		int type = pt->ReadAsInt32();
		switch (type)
		{
		case PTO_S2C_PLAYER_ENTER:
		{
			MSGS2CPlayerEnter msg;
			int namelen = pt->ReadAsInt32();
			msg.name = pt->ReadAsString(namelen);
			msg.scene_id = pt->ReadAsInt32();
			msg.dir = pt->ReadAsInt32();
			msg.role_id = pt->ReadAsInt32();
			msg.weapon_id = pt->ReadAsInt32();
			msg.pos_x = pt->ReadAsFloat();
			msg.pos_y = pt->ReadAsFloat();
			msg.is_local = pt->ReadAsInt32();

			if (scene_find_player(msg.name.c_str()))return;
			scene_add_player(msg.name.c_str(), (int)msg.pos_x, (int)msg.pos_y, msg.dir, msg.role_id, msg.weapon_id);
			if (msg.is_local)
			{
				scene_set_player(msg.name.c_str());
			}
		}
		break;
		case PTO_S2C_CHAT:
		{
			MSGC2SChat chatmsg;
			chatmsg.namelen = pt->ReadAsInt32();
			chatmsg.name = pt->ReadAsString(chatmsg.namelen);
			chatmsg.ctlen = pt->ReadAsInt32();
			chatmsg.content = pt->ReadAsString(chatmsg.ctlen);
			Player* player = scene_find_player(chatmsg.name.c_str());
			if (player) player->Say(chatmsg.content);
		}
		break;
		case PTO_S2C_MOVE_TO_POS:
		{
			int namelen = pt->ReadAsInt32();
			std::string name = pt->ReadAsString(namelen);
			float pos_x = pt->ReadAsFloat();
			float pos_y = pt->ReadAsFloat();
			Player* player = scene_find_player(name.c_str());
			if (player) player->MoveTo(pos_x, pos_y);
		}
		break;
		default:
			break;
		}
		delete pt;
	}
}

void NetThread::Deinit()
{
	g_Client->Disconnect();
	g_Loop->Quit();
	m_Thread->join();
}

void net_manager_init()
{
	NetThread::GetInstance()->Init();
} 

void net_manager_update()
{
	NetThread::GetInstance()->Update();
}

void net_manager_deinit()
{
	NetThread::GetInstance()->Deinit();
}

void net_send_login_message(MSGC2SLogin* msg)
{
	ezio::Buffer buf;
	buf.Write(msg->type);
	buf.Write(msg->namelen);
	buf.Write(msg->name.data(), msg->namelen);
	buf.Write(msg->scene_id);
	buf.Write(msg->dir);
	buf.Write(msg->role_id);
	buf.Write(msg->weapon_id);
	buf.Write(msg->pos_x);
	buf.Write(msg->pos_y);
	int cnt = (int)buf.readable_size();
	buf.Prepend(cnt);
	g_Client->SendMessageToGame((char*)buf.Peek(), buf.readable_size());

}

void net_send_logout_message(MSGC2SLogout* msg)
{

}

void net_send_move_to_pos_message(std::string name ,float x,float y)
{
	Buffer buf;
	buf.Write((int)PTO_C2S_MOVE_TO_POS);
	buf.Write((int)name.length());
	buf.Write(name.data(), name.size());
	buf.Write(x);
	buf.Write(y);
	buf.Prepend((int)buf.readable_size());
	g_Client->SendMessageToGame((char*)buf.Peek(), buf.readable_size());
}

void net_send_chat_message(std::string player, std::string content)
{
	Buffer buf;
	buf.Write((int)PTO_C2S_CHAT);
	buf.Write((int)player.length());
	buf.Write(player.data(),player.size());
	buf.Write((int)content.length());
	buf.Write(content.data(), content.size());
	buf.Prepend((int)buf.readable_size());
	g_Client->SendMessageToGame((char*)buf.Peek(), buf.readable_size());
}

void net_connect_to_server()
{

}

void net_manager_connect()
{
	if (g_Client != nullptr)
	{
		g_Client->Connect();
	}
}

void luaopen_net(lua_State* L)
{
	script_system_register_function(L, net_manager_init);
	script_system_register_function(L, net_manager_update);
	script_system_register_function(L, net_manager_deinit);
}
