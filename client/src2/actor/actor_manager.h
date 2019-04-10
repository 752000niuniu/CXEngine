#pragma once

#include "Singleton.h"

class ActorManager : public Singleton<ActorManager>
{
protected:
	ActorManager();
	~ActorManager();
public:
	void Init();
	void Update();

};

#define ACTOR_MANAGER_INSTANCE ActorManager::GetInstance()