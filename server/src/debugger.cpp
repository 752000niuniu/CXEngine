#include "debugger.h"
#include <functional>
#include "lobject.h"
#include "lstate.h"
#include "script_system.h"
#include "DebugProtocol.h"


#define CALLINFO_MAX_DEPTH 256
using namespace ezio;
char* LINE_ENDING_WIN = "\r\n\r\n";
char* LINE_ENDING_POSIX = "\n\n";

//char* DBG_LINE_ENCODING = nullptr;

void debug_fetch_stacks(lua_State*L, Json&body, int  startFrame, int threadId, int levels)
{
	if (!L)return;
	body["stackFrames"] = Json::array();
	auto& stackFrames = body["stackFrames"];

	while (true)
	{
		if (startFrame > levels)
			break;
		lua_Debug ar;
		if (lua_getstack(L, startFrame, &ar) && lua_getinfo(L, "Slntu", &ar))
		{
			Json sf(Json::object());
			sf["column"] = 0;
			sf["id"] = startFrame;
			sf["line"] = ar.currentline;
			sf["name"] = ar.name == nullptr ? "" : ar.name;

			if (ar.what[0] != 'C')
			{
				std::string sname(ar.short_src);
				sname = sname.substr(sname.find_last_of("/") + 1);
				//td::string fpath = ar.short_src;
				//std::replace(fpath.begin(), fpath.end(), '/', '\\');
				sf["source"] =
				{
					{ "adapterData", "Lua" },
				{ "name" , sname.c_str() },
				{ "path" , ar.short_src},
				{ "sourceReference" , 0 }
				};
			}
			else
			{
				std::string sname(ar.short_src);
				sname = sname.substr(sname.find_last_of("/") + 1);
				sf["source"] =
				{
					{ "adapterData", "C" },
					{ "name" , sname },
					{ "path" , R"(E:/Github/SimpleEngine/scripts/client/main.lua)"},
					{ "sourceReference" , 0 }
				};
			}

			stackFrames.push_back(sf);

			startFrame++;
		}
		else
		{
			startFrame++;
			//	break;
		}
	}
	body["totalFrames"] = stackFrames.size();
	String msg = stackFrames.dump();
	std::cout << msg << std::endl;
}

std::deque<DebuggerMessage> g_ReadQueue;
std::mutex g_ReadQueueMutex;
int g_MsgIdGen = 1;
MessageHandler* handler;


DebuggerMessage::DebuggerMessage(std::string msg, int type)
{
	this->content = msg;
	this->id = g_MsgIdGen;
	this->type = type;
}

void DebuggerMessage::log()
{
	/*std::cout << "msg id:" << id << std::endl
	<< " type:" << type << std::endl
	<< " content:" << content << std::endl;*/
	std::cout << content << std::endl;
}

String DebuggerMessage::wrapMsg(String type, Json message)
{
	message["type"] = type;
	message["seq"] = g_MsgIdGen++;
	String msg = message.dump();
	/*std::cout << "readyToSend:" << std::endl
	<< msg << std::endl;*/

	int len = msg.length();
	String wrapped("");
	wrapped = wrapped + "Content-Length: " + std::to_string(len)
		+ DBG_LINE_ENCODING  + msg;
	return wrapped;
}

void MessageDispatcher::Dispatch(lua_State* L, DebuggerMessage _msg, MessageHandler* handler)
{
	String msg = _msg.content;
	Json request = Json::parse(msg);
	auto type = request["type"];
	//std::cout << "type:" << type << " content:" << msg << std::endl;
	static Json s_CurrentThread = Json::object();
	s_CurrentThread["null"] = true;
	static Json s_CurrentStackFrames = Json::object();
	s_CurrentStackFrames["null"] = true;
	static Json s_CurrentScopes = Json::object();
	s_CurrentScopes["null"] = true;
	static Json s_CurrentVars = Json::object();
	s_CurrentVars["null"] = true;

	if (type == "request")
	{
		String command = request["command"];
		if (command == "initialize") {
			//DebuggerCore::GetInstance()->SetBreaked(true);
		}
		else if (command == "launch")
		{
			String rqstr = request.dump();
			onConfidoneFunc = [handler,rqstr](Json& configDoneResp)
			{
				DebuggerCore::GetInstance()->step_out();
			
				vs::LaunchResponse rsp;
				rsp.setRq(Json::parse(rqstr));
				handler->SendResponse(rsp.js);


				vs::StoppedEvent stopEvent;
				stopEvent.js["event"] = "stopped";
				stopEvent.js["body"]["reason"] = "entry";
				stopEvent.js["body"]["threadId"] = MAIN_THREAD_ID;
				handler->SendEvent(stopEvent.js);
				handler->SendResponse(configDoneResp);
				handler->m_ConfigDone = true;

				//	handler->StopOnBreakPointEvent({});
				//	handler->SendEvent(pauseEvent.js);
				//  DebuggerCore::GetInstance()->SetBreaked(false);
			};
		}
		else if (command == "configurationDone")
		{
			vs::ConfigurationDoneResponse rsp;
			rsp.setRq(request);
			if (onConfidoneFunc)
				onConfidoneFunc(rsp.js);

		}
		else if (command == "setBreakpoints")
		{
			//1._runtime._clearBreakPoints
			//2.verify bp
			//3.sendResponse(bp)
			Json args = request["arguments"];
			String path = args["source"]["path"];
			DebuggerCore::GetInstance()->SetSources(path, args["source"]);
			DebuggerCore::GetInstance()->SetAllBreakPoints(path, args["breakpoints"]);
			DebuggerCore::GetInstance()->VerifyAllBreakPoints();

			vs::SetBreakpointsResponse rsp;
			rsp.setRq(request);

			const Json& bps = DebuggerCore::GetInstance()->GetAllBreakPoints(path);
			rsp.js["body"] = { "breakpoints", bps };

			handler->SendResponse(rsp.js);
		}
		else if (command == "continue")
		{
			vs::Response rsp;
			rsp.setRq(request);
			handler->SendResponse(rsp.js);
			DebuggerCore::GetInstance()->SetBreaked(false);
		}
		else if (command == "next") {
			vs::Response rsp;
			rsp.setRq(request);
			handler->SendResponse(rsp.js);
			DebuggerCore::GetInstance()->step_over();
		}
		else if (command == "stepIn") {
			vs::Response rsp;
			rsp.setRq(request);
			handler->SendResponse(rsp.js);
			DebuggerCore::GetInstance()->step_in();
		}
		else if (command == "stepOut") {
			vs::Response rsp;
			rsp.setRq(request);
			handler->SendResponse(rsp.js);
			DebuggerCore::GetInstance()->step_out();
		}
		else if (command == "stepOver") {
			vs::Response rsp;
			rsp.setRq(request);
			handler->SendResponse(rsp.js);
			DebuggerCore::GetInstance()->step_over();
		}
		else if (command == "pause") {
			vs::Response rsp;
			rsp.setRq(request);
			handler->SendResponse(rsp.js);

			vs::StoppedEvent stopEvent;
			stopEvent.js["body"]["reason"] = "pause";
			DebuggerCore::GetInstance()->SetBreaked(true);

			handler->SendEvent(stopEvent.js);
		}
		else if (command == "stackTrace") {
			vs::Response rsp;
			auto& args = request["arguments"];
			auto& body = rsp.js["body"];
			auto& stackFrames = body["stackFrames"];
			int startFrame = args["startFrame"];
			int threadId = args["threadId"];
			int levels = args["levels"];

			debug_fetch_stacks(L, body, startFrame, threadId, levels);

			rsp.setRq(request);

			s_CurrentStackFrames["stackFrames"] = rsp.js["body"]["stackFrames"];

			handler->SendResponse(rsp.js);
		}
		else if (command == "scopes") {

			auto& args = request["arguments"];
			int frameId = args["frameId"];

			vs::Response rsp;
			auto& body = rsp.js["body"];
			auto& scopes = body["scopes"];
			scopes = Json::array();
			scopes[0] = {
				{ "expensive", false },
			{ "name","Local" },
			{ "variablesReference",1000 + frameId }
			};
			rsp.setRq(request);

			/*s_CurrentStackFrames["stackFrames"] =
			rsp.js["body"]["stackFrames"];*/

			handler->SendResponse(rsp.js);
		}
		else if (command == "variables") {
			//	auto fecth_vars = call_lua_function(L, "fetch_locals", 4);
			//	std::cout << any_cast<const char*>(fecth_vars[0]) << std::endl;
			//
			auto& args = request["arguments"];
			int frameId = args["variablesReference"];
			frameId = frameId - 1000;

			vs::Response rsp;
			auto& body = rsp.js["body"];
			auto& variables = body["variables"];
			variables = Json::array();
			if (!L)return;
			std::vector<any> fecth_vars = call_lua_function(L, "fetch_locals", frameId + 2);
			if (fecth_vars.size() > 0)
			{
				const char* p = any_cast<const char*>(fecth_vars[0]);
				//std::cout << p << std::endl;
				Json var_arrys = Json::parse(p);
				int index = 0;
				for (Json& var : var_arrys)
				{
					String ktype = var[0];
					String key = var[1];
					String value = var[2];
					String vtype = var[3];

					variables[index++] =
					{
						{ "name",key },
					{ "type",vtype },
					{ "value",value },
					{ "variablesReference" ,frameId }
					};
				}
			}

			//			rsp.js = Json::parse(R"#({"seq":24,"type":"response","request_seq":12,"command":"variables","success":true,"body":{"variables":[{"name":"local_0_i","type":"integer","value":"123","variablesReference":0},{"name":"local_0_f","type":"float","value":"3.14","variablesReference":0},{"name":"local_0_s","type":"string","value":"hello world","variablesReference":0},{"name":"local_0_o","type":"object","value":"Object","variablesReference":1002}]}})#");
			rsp.setRq(request);

			handler->SendResponse(rsp.js);
		}else if (command == "threads") {
			vs::ThreadsResponse rsp;
			rsp.setRq(request);
			Json threads_js = Json::array({
				{
					{ "id",MAIN_THREAD_ID },
				{ "name","thread 1" }
				}
				});
			rsp.js["body"]["threads"] = {
				threads_js
			};

			s_CurrentThread["threads"] = {
				threads_js
			};
			handler->SendResponse(rsp.js);
		}
	}
}

MessageHandler::MessageHandler()
	: m_ConfigDone(false), CurrentLine(1)
{
	m_Dispatcher = new MessageDispatcher();
}

MessageHandler::~MessageHandler()
{
	delete m_Dispatcher;
}

void MessageHandler::Loop(lua_State*L)
{
	if (!g_ReadQueue.empty())
	{
		g_ReadQueueMutex.lock();
		auto msg = g_ReadQueue.front();
		g_ReadQueue.pop_front();
		g_ReadQueueMutex.unlock();
		std::cout << "[g_ReadQueue Log!!!!!!!!!!!!!]" << std::endl;
		msg.log();
		std::cout << std::endl;
		m_Dispatcher->Dispatch(L, msg, this);
	}
}

void MessageHandler::StopOnBreakPointEvent(Json bp)
{
	vs::StoppedEvent ev;
	ev.js["event"] = "stopped";
	ev.js["body"] = {
		{ "reason","breakpoint" },
	{ "threadId",MAIN_THREAD_ID }
	};
	SendEvent(ev.js);
}

void MessageHandler::EnReadQueue(const std::string& msgstr)
{
	g_ReadQueue.push_back(DebuggerMessage(msgstr));
	Loop(script_system_get_luastate());
}

bool MessageHandler::SendEvent(Json& msgJson)
{
	DebuggerMessage msg("");
	msg.content = msg.wrapMsg("event", msgJson);
	Send(msg);
	return true;
}

bool MessageHandler::SendResponse(Json& msgJson)
{
	DebuggerMessage msg("");
	msg.content = msg.wrapMsg("response", msgJson);
	Send(msg);
	return true;
}

bool MessageHandler::SendRequest(Json& msgJson)
{
	DebuggerMessage msg("");
	msg.content = msg.wrapMsg("request", msgJson);
	Send(msg);
	return true;
}

void MessageHandler::Send(DebuggerMessage msg)
{
	if (m_Connection != nullptr)
	{
		std::cout << "send msg:" << msg.content << std::endl;
		m_Connection->Send(msg.content);
	}
}


DebuggerCore::DebuggerCore()
{
	m_StepDepth = 0;
	m_StepInto = false;
	m_TrapWait = false;
	m_Breaked = false;
	m_handler = nullptr;
}

DebuggerCore::~DebuggerCore()
{

}

void DebuggerCore::SetHook(lua_State*L)
{
	m_L = L;
	lua_sethook(
		L,
		DebugHookLoop,
		LUA_MASKLINE | LUA_MASKCALL | LUA_MASKRET, 4096);
}

bool DebuggerCore::breaked(lua_State*L)
{
	// while handle debug update
	auto* dbgCore = GetInstance();

	while (m_TrapWait)
	{
		Sleep(10);
	}
	return false;
}

bool DebuggerCore::hasBreakPoint(const char* source)
{
	return true;
}

void DebuggerCore::step_in()
{
	m_TrapWait = false;
	m_StepInto = true;
}

void DebuggerCore::step_out()
{
	m_TrapWait = false;
	m_StepDepth = (size_t)(m_L)+1;
}

void DebuggerCore::step_over()
{
	m_TrapWait = false;
	m_StepDepth = (size_t)(m_L);
}

void DebugHookLoop(lua_State* L, lua_Debug* ar)
{
	auto* dbgCore = DebuggerCore::GetInstance();
	switch (ar->event)
	{
	case LUA_HOOKLINE:
		OnHookLine(L, ar);
		break;
	case LUA_HOOKCALL:
		if (dbgCore->m_StepDepth &&
			(size_t)L + CALLINFO_MAX_DEPTH > dbgCore->m_StepDepth &&
			(size_t)L - CALLINFO_MAX_DEPTH < dbgCore->m_StepDepth)
			dbgCore->m_StepDepth++;
		break;
	case LUA_HOOKRET:
		if (dbgCore->m_StepDepth &&
			(size_t)L + CALLINFO_MAX_DEPTH > dbgCore->m_StepDepth &&
			(size_t)L - CALLINFO_MAX_DEPTH < dbgCore->m_StepDepth)
			dbgCore->m_StepDepth--;
		break;
	default:
		break;
	}
}

bool hitBreakPointTest(lua_Debug* ar, int currentline)
{
	return false;
}

void OnHookLine(lua_State* L, lua_Debug* ar)
{
	auto* dbgCore = DebuggerCore::GetInstance();
	auto* m_handler = DebuggerCore::GetInstance()->m_handler;
	LClosure* active_closure;
	CallInfo* active_callinfo = ar->i_ci;
	if (!ttisLclosure(active_callinfo->func))
		return;
	active_closure = clLvalue(active_callinfo->func);
	lua_getinfo(L, "nSlf", ar);
	if (!dbgCore->hasBreakPoint(ar->source))
	{
		return;
	}

	if (ar->what[0] == 'C')
		return;
	assert(lua_isfunction(L, -1));

	String path = ar->source;
	if (path[0] == '@')
		path = path.substr(1);
	std::replace(path.begin(), path.end(), '\\', '/');
	if (m_handler&&handler->m_ConfigDone)
	{
		if (dbgCore->m_StepInto) {
			dbgCore->m_TrapWait = true;
			dbgCore->m_StepInto = dbgCore->m_StepDepth = 0;

			Json bp = dbgCore->get_bp(path, ar->currentline);
			m_handler->StopOnBreakPointEvent(bp);
			m_handler->CurrentLine = ar->currentline;
			dbgCore->breaked(L);
		}
		else if (dbgCore->m_StepDepth
			&&dbgCore->m_StepDepth <= (size_t)L
			&&dbgCore->m_StepDepth >= (size_t)L - CALLINFO_MAX_DEPTH)
		{

			dbgCore->m_StepInto = dbgCore->m_StepDepth = 0;
			dbgCore->m_TrapWait = true;

			Json bp = dbgCore->get_bp(path, ar->currentline);
			m_handler->StopOnBreakPointEvent(bp);
			m_handler->CurrentLine = ar->currentline;
			dbgCore->breaked(L);

		}

		if (dbgCore->hit_bp(path, ar->currentline))
		{
			dbgCore->m_StepInto = dbgCore->m_StepDepth = 0;
			dbgCore->m_TrapWait = true;
			Json bp = dbgCore->get_bp(path, ar->currentline);
			m_handler->StopOnBreakPointEvent(bp);
			m_handler->CurrentLine = ar->currentline;
			dbgCore->breaked(L);

		}
	}
	else
	{
		if (dbgCore->m_StepInto) {
			dbgCore->m_TrapWait = true;
			dbgCore->m_StepInto = dbgCore->m_StepDepth = 0;
			dbgCore->breaked(L);
		}

	}
}

/************************************************************************/
/* 接收debugAdapter的消息 操作程序                                                                     
*/
/************************************************************************/
void debugger_run_session(int port)
{
	EventLoop loop;

	SocketAddress addr(port);
	TCPServer server(&loop, addr, "DebuggerRuntime");
	server.set_on_connection([](const TCPConnectionPtr& conn) {
		std::cout << "connection " << (conn->connected() ? "true" : "false") << std::endl;
		handler = new MessageHandler();
		handler->SetConnection(conn);
		DebuggerCore::GetInstance()->SetHandler(handler);
	});

	server.set_on_message([](const TCPConnectionPtr& conn, Buffer& buf, TimePoint ts) {
		while (buf.end() != buf.begin())
		{
			std::string preview(buf.cbegin(), buf.cend());
			std::cout << "on message :" << preview << std::endl;
			int contentLen = preview.find_first_of(DBG_LINE_ENCODING);
			if (contentLen == std::string::npos)
			{
				break;
			}
			else
			{
				int start = 16;
				int end = contentLen;
				auto lenstr = preview.substr(start, end - start);
				int len = std::stoi(lenstr.c_str());
				if (preview.size() < contentLen + strlen(DBG_LINE_ENCODING) + len)break;
				buf.Consume(contentLen + strlen(DBG_LINE_ENCODING));
				auto json = buf.ReadAsString(len);
				std::cout << "on message :" << json << std::endl;
				handler->EnReadQueue(json);
			}
		}
	});

	server.Start();
	printf("%s is running at %s\n", server.name().c_str(), server.ip_port().c_str());

	loop.Run();
}

std::thread* g_thread;
bool Debugging = false;
void debugger_start_session()
{
	auto debug = script_system_get_config("--debug");
	Debugging = debug == "1";
	auto LE = script_system_get_config("--LE");
	if (LE == "LF")
		DBG_LINE_ENCODING = LINE_ENDING_POSIX;
	else //CRLF
		DBG_LINE_ENCODING = LINE_ENDING_WIN ;

	if (Debugging)
	{
		auto portstr = script_system_get_config("debugPort");
		int port = 0;
		if (portstr != "")
		{
			port = std::stoi(portstr);
		}

		

		g_thread = new std::thread(&debugger_run_session, port);
		DebuggerCore::GetInstance()->SetHook(script_system_get_luastate());
		DebuggerCore::GetInstance()->step_in();
	}
	
}

void debugger_stop_session()
{
	if (Debugging)
	{
		g_thread->join();
	}
}

void debugger_update_session()
{
	if (Debugging)
	{
		handler->Loop(script_system_get_luastate());
	}
}


void luaopen_debugger(lua_State* L)
{
	script_system_register_function(L, debugger_start_session);
	script_system_register_function(L, debugger_update_session);
	script_system_register_function(L, debugger_stop_session);
}

