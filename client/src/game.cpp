#include "game.h"	
#include "script_system.h"
#include "window.h"
#include "scene/scene_manager.h"
#include "scene/base_scene.h"
#include "file_system.h"
#include "actor/action.h"
#include "scene/game_map.h"
#include "resource_manager.h"
#include "utils.h"


Game::Game()
{
	
}

Game::~Game()
{

}

Pos Game::MapPosToScreenPos(Pos mapPos)
{
	if (IsRunning())
	{
		GameMap* map = SCENE_MANAGER_INSTANCE->GetCurrentScene()->GetGameMap();
		return { mapPos.x + map->GetMapOffsetX(),mapPos.y + map->GetMapOffsetY() };
	}
	else
	{
		return mapPos;
	}
}

Pos Game::ScreenPosToMapPos(Pos screenPos)
{
	if (IsRunning())
	{
		GameMap* map = SCENE_MANAGER_INSTANCE->GetCurrentScene()->GetGameMap();
		return { screenPos.x - map->GetMapOffsetX(),screenPos.y - map->GetMapOffsetY() };
	}
	else
	{
		return screenPos;
	}
}

bool Game::IsRunning()
{
	auto* curScene = SCENE_MANAGER_INSTANCE->GetCurrentScene();
	return curScene->GetGameMap() != nullptr;
}

int util_screen_pos_to_map_pos(lua_State* L) {
	float mx = (float)lua_tonumber(L, 1);
	float my = (float)lua_tonumber(L, 2);
	Pos pos = GAME_INSTANCE->ScreenPosToMapPos(Pos(mx, my));
	lua_pushnumber(L, pos.x);
	lua_pushnumber(L, pos.y);
	return 2;
}

int util_gb2312_to_utf8(lua_State* L) {
	const char* str = lua_tostring(L, 1);
	std::string s = utils::GB2312ToUtf8(str);
	lua_pushstring(L, s.c_str());
	return 1;
}

void luaopen_game(lua_State* L) {
	script_system_register_luac_function(L, util_screen_pos_to_map_pos);
	script_system_register_luac_function(L, util_gb2312_to_utf8);
}