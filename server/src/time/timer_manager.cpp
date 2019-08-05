#include "timer_manager.h"
#include "timer.h"
#include <algorithm>

#include "logger.h"
#include "script_system.h"


TimerManager::TimerManager()
{

}

TimerManager::~TimerManager()
{

}

void TimerManager::Update()
{
	//clean up the trash
	for (auto & it : m_GarbageContainer)
	{
		m_TimerContainer.erase(it.first);
	}
	m_GarbageContainer.clear();

	for (auto it = m_TimerContainer.begin(); it != m_TimerContainer.end();)
	{
		if (it->second.Update())
		{
			m_TimerContainer.erase(it++);
		}
		else
		{
			++it;
		}
	}
}

bool TimerManager::CreateTimer(const std::string & name, float32 targetTime,
	bool countingDown, bool loop,
	std::function<void()> func, bool paused)
{
	for (auto & it : m_TimerContainer)
	{
		if (it.first == name)
		{
			return false;
		}
	}

	for (auto it = m_TimerContainer.begin(); it != m_TimerContainer.end(); ++it)
	{
		if (it->first == name)
		{
			m_GarbageContainer[it->first] = it->second;
			break;
		}
	}

	Timer newTimer(targetTime, countingDown, loop, func, paused);
	m_TimerContainer[name] = newTimer;
	return true;
}

bool TimerManager::RemoveTimer(const std::string & name)
{
	for (auto it = m_TimerContainer.begin(); it != m_TimerContainer.end(); ++it)
	{
		if (it->first == name)
		{
			m_GarbageContainer[it->first] = it->second;
			return true;
		}
	}

	cxlog_warn(std::string(_T("TimerManager::RemoveTimer: trying to remove unknown timer '") + name + _T("'.")).c_str());
	return false;
}

void TimerManager::PauseTimer(const std::string & name, bool paused)
{
	for (auto & it : m_TimerContainer)
	{
		if (it.first == name)
		{
			it.second.SetPaused(paused);
			return;
		}
	}
	cxlog_warn(std::string(_T("TimerManager::PauseTimer: trying to pause unknown timer '") + name + _T("'.")).c_str());
}

void TimerManager::SetCountingDownTimer(const std::string & name, bool countingDown)
{
	for (auto & it : m_TimerContainer)
	{
		if (it.first == name)
		{
			it.second.SetCountingDown(countingDown);
			return;
		}
	}
	cxlog_warn(std::string(_T("TimerManager::SetCountingDownTimer: trying to adjust unknown timer '") + name + _T("'.")).c_str());
}

void TimerManager::SetLoopTimer(const std::string & name, bool looping)
{
	for (auto & it : m_TimerContainer)
	{
		if (it.first == name)
		{
			it.second.SetLoop(looping);
			return;
		}
	}
	cxlog_warn(std::string(_T("TimerManager::SetLoopTimer: trying to adjust unknown timer '") + name + _T("'.")).c_str());
}

void TimerManager::ResetTimer(const std::string & name, bool paused)
{
	for (auto & it : m_TimerContainer)
	{
		if (it.first == name)
		{
			it.second.Reset(paused);
			return;
		}
	}
	cxlog_warn(std::string(_T("TimerManager::ResetTimer: trying to reset unknown timer '") + name + _T("'.")).c_str());
}

void TimerManager::SetTargetTimeTimer(const std::string & name, float32 targetTime, bool reset, bool paused)
{
	for (auto & it : m_TimerContainer)
	{
		if (it.first == name)
		{
			it.second.SetTargetTime(targetTime, reset, paused);
			return;
		}
	}
	cxlog_warn(std::string(_T("TimerManager::SetTargetTimeTimer: trying to adjust unknown timer '") + name + _T("'.")).c_str());
}

void TimerManager::SetFunctionTimer(const std::string & name, const std::function<void()> & func)
{
	for (auto & it : m_TimerContainer)
	{
		if (it.first == name)
		{
			it.second.SetFunction(func);
			return;
		}
	}
	cxlog_warn(std::string(_T("TimerManager::SetFunctionTimer: trying to adjust unknown timer '") + name + _T("'.")).c_str());
}

float64 TimerManager::ForceEndTimer(const std::string & name)
{
	for (auto & it : m_TimerContainer)
	{
		if (it.first == name)
		{
			return it.second.ForceEnd();
		}
	}
	cxlog_warn(std::string(_T("TimerManager::ForceEndTimer: trying to end unknown timer '") + name + _T("'.")).c_str());
	return 0;
}

void TimerManager::ForwardTimer(const std::string & name, float64 time)
{
	for (auto & it : m_TimerContainer)
	{
		if (it.first == name)
		{
			it.second.Forward(time);
			return;
		}
	}
	cxlog_warn(std::string(_T("TimerManager::ForwardTimer: trying to forward unknown timer '") + name + _T("'.")).c_str());
}

void TimerManager::ForwardAllTimers(float64 time)
{
	for (auto & it : m_TimerContainer)
	{
		it.second.Forward(time);
	}
}

int32 TimerManager::GetTimerMinutes(const std::string & name) const
{
	for (auto & it : m_TimerContainer)
	{
		if (it.first == name)
		{
			return it.second.GetCurrentMinutes();
		}
	}
	cxlog_warn(std::string(_T("TimerManager::GetTimerMinutes: Couldn't find the timer '") + name + _T("'.")).c_str());
	return 0;
}

int32 TimerManager::GetTimerSeconds(const std::string & name) const
{
	for (auto & it : m_TimerContainer)
	{
		if (it.first == name)
		{
			return it.second.GetCurrentSeconds();
		}
	}
	cxlog_warn(std::string(_T("TimerManager::GetTimerSeconds: Couldn't find the timer '") + name + _T("'.")).c_str());
	return 0;
}

int32 TimerManager::GetTimerTotalSeconds(const std::string & name) const
{
	for (auto & it : m_TimerContainer)
	{
		if (it.first == name)
		{
			return it.second.GetCurrentTotalSeconds();
		}
	}
	cxlog_warn(std::string(_T("TimerManager::GetTimerTotalSeconds: Couldn't find the timer '") + name + _T("'.")).c_str());
	return 0;
}

float64 TimerManager::GetTimerTargetTime(const std::string & name) const
{
	for (auto & it : m_TimerContainer)
	{
		if (it.first == name)
		{
			return it.second.GetTargetTime();
		}
	}
	cxlog_warn(std::string(_T("TimerManager::GetTimerTargetTime: trying to access unknown timer '") + name + _T("'.")).c_str());
	return 0;
}

float64 TimerManager::GetTimerAccurateTime(const std::string & name) const
{
	for (auto & it : m_TimerContainer)
	{
		if (it.first == name)
		{
			return it.second.GetCurrentAccurateTime();
		}
	}
	cxlog_warn(std::string(_T("TimerManager::GetTimerAccurateTime: Couldn't find the timer '") + name + _T("'.")).c_str());
	return 0;
}


void timer_manager_init()
{
	TIMER_MANAGER_INTANCE; //init
}


void timer_manager_update()
{
	TIMER_MANAGER_INTANCE->Update();
}
void timer_manager_deinit()
{
	TIMER_MANAGER_INTANCE->DeleteSingleton();
}


void luaopen_timer_manager(lua_State* L)
{
	script_system_register_function(L, timer_manager_init);
	script_system_register_function(L, timer_manager_update);
	script_system_register_function(L, timer_manager_deinit);
}