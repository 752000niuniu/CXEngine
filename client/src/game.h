#pragma once
#include <string>
#include "engine.h"
#include "singleton.h"
#include "pos.h"
/*
负责初始化Engine Update Draw
*/
class Game : public Singleton<Game>
{
public:
	Game();
	~Game();
	Pos MapPosToScreenPos(Pos mapPos);
	Pos ScreenPosToMapPos(Pos screenPos);
	uint32_t GetActionWasID(int type, int roleID, int actionID);
	uint32_t GetWeaponWasID(int weaponID, int actionID);
	int GetRoleIDByName(int actorType, const char* templ_name);
private:
	bool IsRunning();
};

#define GAME_INSTANCE Game::GetInstance()



