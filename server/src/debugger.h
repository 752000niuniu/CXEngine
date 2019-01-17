#pragma once
#include "defines.h"
#include "defineTypes.h"

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

static char* DBG_LINE_ENCODING; 
//#define DBG_LINE_ENCODING "\r\n\r\n"


void DebugHookLoop(lua_State* L, lua_Debug* ar);
bool hitBreakPointTest(lua_Debug* ar, int currentline);
void OnHookLine(lua_State* L, lua_Debug* ar);

struct DebuggerMessage
{
	int id;
	int type;
	std::string content;
	DebuggerMessage() {};
	DebuggerMessage(std::string msg, int type = 0);

	String wrapMsg(String type, nlohmann::json message);
	void log();
};


class MessageDispatcher;
class MessageHandler
{
public:
	MessageHandler();

	~MessageHandler();
	void SetConnection(ezio::TCPConnectionPtr connection) { m_Connection = connection; };
	void Loop(lua_State*L);

	bool SendEvent(nlohmann::json& msg);

	bool SendResponse(nlohmann::json& msg);

	bool SendRequest(nlohmann::json& msg);
	void Send(DebuggerMessage msg);

	void SendRaw(std::string msg) { if (m_Connection) { m_Connection->Send(msg); } };

	void StopOnBreakPointEvent(Json bp);
	bool m_ConfigDone;
	int CurrentLine = 0;
	void EnReadQueue(const std::string& msgstr);
	ezio::TCPConnectionPtr Conn() { return m_Connection; };
private:
	MessageDispatcher* m_Dispatcher;
	ezio::TCPConnectionPtr m_Connection;
};

class MessageDispatcher
{
public:
	void Dispatch(lua_State* L, DebuggerMessage msg, MessageHandler* handler);
	std::function<void(Json& )> onConfidoneFunc;
};

class DebuggerCore
{
public:
	DebuggerCore();

	~DebuggerCore();

	bool breaked(lua_State*L);

	bool hasBreakPoint(const char* source);

	void SetHook(lua_State*L);

	void SetBreaked(bool isBreak) {
		m_TrapWait = isBreak;
	};
	
	static DebuggerCore* GetInstance()
	{
		static DebuggerCore* instance = new DebuggerCore();
		return instance;
	}

	void SetHandler(MessageHandler* handler)
	{
		m_handler = handler;
	}

	void SetAllBreakPoints(String path, Json bps)
	{
		//	std::cout << bps << std::endl;
		assert(bps.is_array());
		format_path(path);

		m_AllBreakPoints[path] = Json::array();
		Json& new_bps = m_AllBreakPoints[path];
		for (Json::iterator bp_it = bps.begin(); bp_it != bps.end(); ++bp_it)
		{
			Json new_bp = Json::object();
			new_bp["id"] = 0;
			new_bp["verified"] = false;
			new_bp["line"] = bp_it.value()["line"];
			new_bps.push_back(new_bp);
		}
	}

	const auto& GetAllBreakPoints()
	{
		return m_AllBreakPoints;
	}

	const Json& GetAllBreakPoints(String path)
	{
		format_path(path);
		assert(m_AllBreakPoints[path].is_array());
		return m_AllBreakPoints[path];
	}

	void VerifyAllBreakPoints()
	{
		int id = 1;
		for (auto& pb_pair : m_AllBreakPoints)
		{
			auto& bp_ary = pb_pair.second;
			for (Json::iterator bp_it = bp_ary.begin(); bp_it != bp_ary.end(); ++bp_it)
			{
				bp_it.value()["id"] = id++;
				bp_it.value()["verified"] = true;
			}
		}
		//	std::cout << m_AllBreakPoints << std::endl;
	}

	void SetSources(String path, Json source)
	{
		format_path(path);
		m_Sources.push_back(source);
	}
	bool has_bp(String path)
	{
		format_path(path);
		return m_AllBreakPoints.find(path) != m_AllBreakPoints.end();
	};

	bool hit_bp(String path, int line)
	{
		format_path(path);
		if (!has_bp(path))return false;
		const Json& bp_ary = m_AllBreakPoints[path];
		for (Json::const_iterator bp_it = bp_ary.cbegin(); bp_it != bp_ary.cend(); ++bp_it)
		{
			if (bp_it.value()["line"] == line)
			{
				return true;
			}
		}
		return false;
	}
	Json get_bp(String path, int line)
	{
		format_path(path);
		const Json& bp_ary = m_AllBreakPoints[path];
		for (Json::const_iterator bp_it = bp_ary.cbegin(); bp_it != bp_ary.cend(); ++bp_it)
		{
			if (bp_it.value()["line"] == line)
			{
				return bp_it.value();
			}
		}
		return{};
	}
	void step_in();
	void step_out();
	void step_over();
	inline static void format_path(String& path)
	{
		if (path[0] == '@')
			path = path.substr(1);
		std::replace(path.begin(), path.end(), '\\', '/');
	}
	bool m_StepInto;
	int m_StepDepth;
	bool m_Breaked;
	bool m_TrapWait;
	MessageHandler* m_handler;
private:
	std::vector<Json> m_Sources;
	std::map<String, Json> m_AllBreakPoints;
	lua_State* m_L;
};

void luaopen_debugger(lua_State* L);