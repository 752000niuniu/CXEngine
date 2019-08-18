#include "game.h"	
#include "script_system.h"
#include "window.h"
#include "scene/scene_manager.h"
#include "scene/base_scene.h"
#include "file_system.h"
#include "actor/action.h"
#include "scene/game_map.h"

static utils::tsv* s_AvatarRoleTable;
static utils::tsv* s_AvatarWeapon90Table;
static utils::tsv* s_AvatarWeaponTable;
static utils::tsv* s_AvatarNpcTable;
static utils::tsv* s_AvatarBBTable;
Game::Game()
{
	s_AvatarRoleTable = new  utils::tsv(FileSystem::GetTablePath("avatar_role.1.tsv"));
	s_AvatarWeapon90Table = new  utils::tsv(FileSystem::GetTablePath("avatar_weapon.1.tsv"));
	s_AvatarWeaponTable = new  utils::tsv(FileSystem::GetTablePath("avatar_weapon.2.tsv"));
	s_AvatarNpcTable = new  utils::tsv(FileSystem::GetTablePath("avatar_npc.1.tsv"));
	s_AvatarBBTable = new  utils::tsv(FileSystem::GetTablePath("avatar_bb.1.tsv"));
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


uint32_t Game::GetActionWasID(int type, int roleID, int actionID)
{
	if (actionID < 0) return -1;
	if (roleID < 0) return -1;
	
	auto* rowTable = s_AvatarRoleTable;
	switch (type)
	{
	case ACTOR_TYPE_DEFAULT:
		break;
	case ACTOR_TYPE_PLAYER:
		break;
	case ACTOR_TYPE_PET:
		rowTable = s_AvatarBBTable;
		break;
	case ACTOR_TYPE_NPC:
		rowTable = s_AvatarNpcTable;
		break;
	default:
		break;
	}
	std::string wasIDstr("");
	if (actionID == ACTION_BATIDLE)
	{
		auto wasIdle = rowTable->Rows[roleID][action_get_name(ACTION_IDLE)];
		auto wasBatidle = rowTable->Rows[roleID][action_get_name(ACTION_BATIDLE)];
		if (wasBatidle != "")wasIDstr = wasBatidle;
		else wasBatidle = wasIdle;
	}
	else
	{
		wasIDstr = rowTable->Rows[roleID][action_get_name(actionID)];
	}
	if (wasIDstr == "")
	{
		return-1;
	}
	uint32 wasID = std::stoul(wasIDstr, 0, 16);
	return wasID;
}

uint32_t Game::GetWeaponWasID(int weaponID, int actionID)
{
	if (actionID < 0) return -1;
	if (weaponID < 0) return -1;


	std::string wasIDstr("");
	if (actionID == ACTION_BATIDLE)
	{
		auto wasIdle = s_AvatarWeapon90Table->Rows[weaponID][action_get_name(ACTION_IDLE)];
		auto wasBatidle = s_AvatarWeapon90Table->Rows[weaponID][action_get_name(ACTION_BATIDLE)];
		if (wasIdle != "")wasIDstr = wasBatidle;
		else wasBatidle = wasIdle;
	}
	else
	{
		wasIDstr = s_AvatarWeapon90Table->Rows[weaponID][action_get_name(actionID)];
	}
	if (wasIDstr == "")return-1;
	uint32 wasID = std::stoul(wasIDstr, 0, 16);
	return wasID;
}

int Game::GetRoleIDByName(int actorType, const char* templ_name)
{
	auto* rowTable = s_AvatarRoleTable;
	switch (actorType)
	{
	case ACTOR_TYPE_DEFAULT:
		break;
	case ACTOR_TYPE_PLAYER:
		break;
	case ACTOR_TYPE_PET:
		rowTable = s_AvatarBBTable;
		break;
	case ACTOR_TYPE_NPC:
		rowTable = s_AvatarNpcTable;
		break;
	default:
		break;
	}
	for (size_t i = 0; i < rowTable->Rows.size(); i++)
	{
		auto& row = rowTable->Rows[i];
		if (row["ID"] == templ_name)
		{
			return (int)i;
		}
	}
	return 0;
}

int util_screen_pos_to_map_pos(lua_State* L) {
	float mx = (float)lua_tonumber(L, 1);
	float my = (float)lua_tonumber(L, 2);
	Pos pos = GAME_INSTANCE->ScreenPosToMapPos(Pos(mx, my));
	lua_pushnumber(L, pos.x);
	lua_pushnumber(L, pos.y);
	return 2;
}


int game_get_action_was_id(lua_State* L)
{
	auto type = (int)lua_tointeger(L, 1);
	auto roleID = (int)lua_tointeger(L, 2);
	auto actionID = (int)lua_tointeger(L, 3);
	auto wasid = GAME_INSTANCE->GetActionWasID(type, roleID, actionID);
	lua_pushinteger(L, wasid);
	return 1;
}

int game_get_weapon_was_id(lua_State* L)
{
	auto weaponID = (int)lua_tointeger(L, 1);
	auto actionID = (int)lua_tointeger(L, 2);
	auto wasid = GAME_INSTANCE->GetWeaponWasID(weaponID,  actionID);
	lua_pushinteger(L, wasid);
	return 1;
}


void luaopen_game(lua_State* L) {
	script_system_register_luac_function(L, util_screen_pos_to_map_pos);
	script_system_register_luac_function(L, game_get_action_was_id);
	script_system_register_luac_function(L, game_get_weapon_was_id);

}