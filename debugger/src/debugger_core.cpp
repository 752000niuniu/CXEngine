/*author by: oceancx*/
#include "debugger_core.h"
#include <functional>

#define CALLINFO_MAX_DEPTH 256

DebuggerCore::DebuggerCore()
{
	m_StepDepth = 0;
	m_StepInto = false;
	m_TrapWait = false;
	m_Breaked = false;
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
		/*dbgCore->m_handler->Loop();
		Sleep(10);*/
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
	m_StepDepth = (size_t)(m_L) + 1;
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
	//auto* m_handler = DebuggerCore::GetInstance()->m_handler;
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
	/*if (m_handler&&m_handler->m_ConfigDone)
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
	}*/
}
