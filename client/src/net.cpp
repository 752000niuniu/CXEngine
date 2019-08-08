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

#include "scene/base_scene.h"
#include "kbase/pickle.h"
#include "scene/scene_manager.h"
#include "logger.h"
#include "net_thread_queue.h"
#include "lua_net.h"

using namespace ezio;

NetThreadQueue g_ReadPacketQueue;

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

	void SendMessageToServer(int proto, const char* msg);

	void Disconnect() { m_Client.Disconnect(); };

private:
	void OnConnection(const TCPConnectionPtr&);

	void OnMessage(const TCPConnectionPtr&, Buffer&, TimePoint);

	TCPClient m_Client;
	EventLoop* m_EventLoop;
};


NetClient::NetClient(EventLoop* loop, const SocketAddress& serverAddr,const char* name)
	:m_Client(loop, serverAddr, name),m_EventLoop(loop)
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
	std::string msg(buf, len);
	m_EventLoop->RunTask([this,msg]() {
		if (m_Client.connection() != nullptr && m_Client.connection()->connected())
		{
			cxlog_info("%s", msg.c_str());
			m_Client.connection()->Send(msg);
		}
	});
	
}

void NetClient::SendMessageToServer(int proto, const char* msg)
{
	ezio::Buffer buf;
	buf.Write(proto);
	buf.Write(msg, strlen(msg));
	int cnt = (int)buf.readable_size();
	buf.Prepend(cnt);
	printf("proto:%d msg:%s\n", proto, msg);

	m_EventLoop->RunTask([this, buf]() {
		if (m_Client.connection() != nullptr && m_Client.connection()->connected()) {
			m_Client.connection()->Send(kbase::StringView(buf.Peek(), buf.readable_size()));
		}
	});
}

void NetClient::OnConnection(const TCPConnectionPtr& conn)
{
	const char* state = conn->connected() ? "connected" : "disconnected";
	cxlog_info("Connection %s is %s\n", conn->peer_addr().ToHostPort().c_str(), state);
}

void NetClient::OnMessage(const TCPConnectionPtr& conn, Buffer& buf, TimePoint time)
{

	while (buf.readable_size() >= CX_MSG_HEADER_LEN)
	{
		int len = buf.PeekAsInt32();
		if (buf.readable_size() >= len + CX_MSG_HEADER_LEN)
		{
			buf.Consume(CX_MSG_HEADER_LEN);
			g_ReadPacketQueue.PushBack(NetThreadQueue::Read, buf.Peek(), len);
			buf.Consume(len);
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
	void Update(lua_State* L);
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

void NetThread::Update(lua_State* L )
{
	while (!g_ReadPacketQueue.Empty(NetThreadQueue::Read))
	{
		Buffer& pt = g_ReadPacketQueue.Front(NetThreadQueue::Read);
		printf("%s\n", std::string(pt.Peek(), pt.readable_size()).c_str());
		lua_getglobal(L , "game_dispatch_message");
		lua_push_ezio_buffer(L, pt);
		int res = lua_pcall(L, 1, 0, 0);
		g_ReadPacketQueue.PopFront(NetThreadQueue::Read);
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

int net_manager_update(lua_State*L)
{
	NetThread::GetInstance()->Update(L);
	return 0;
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
	buf.Write((int)x);
	buf.Write((int)y);
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

void net_send_message(int proto, const char* msg) {
	g_Client->SendMessageToServer(proto, msg);
}


void net_manager_reconnect(){
	g_Client->Disconnect();
	g_Client->Connect();
}


void luaopen_net(lua_State* L)
{
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

	script_system_register_function(L, net_manager_init);
	script_system_register_luac_function(L, net_manager_update);
	script_system_register_function(L, net_manager_deinit);

	script_system_register_function(L, net_manager_reconnect);
	script_system_register_function(L, net_send_message);	
}
