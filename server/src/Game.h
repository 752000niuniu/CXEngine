#pragma once
#include <string>
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
	int GetScreenWidth() { return 800; };
	int GetScreenHeight() { return 600; };

	float GetDeltaTime() { return 1000/60.f; };

private:
	bool IsRunning();
};

#define GAME_INSTANCE Game::GetInstance()



