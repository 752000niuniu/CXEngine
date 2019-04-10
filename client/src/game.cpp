#include "Game.h"	
#include "script_system.h"
#include "Window.h"
#include "scene/scene_manager.h"
#include "scene/base_scene.h"
#include "../server/src/file_system.h"

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

static utils::tsv s_AvatarRoleTable(FileSystem::GetTablePath("avatar_role.1.tsv"));
static utils::tsv s_AvatarWeapon90Table(FileSystem::GetTablePath("avatar_weapon.1.tsv"));
static utils::tsv s_AvatarWeaponTable(FileSystem::GetTablePath("avatar_weapon.2.tsv"));
static utils::tsv s_AvatarNpcTable(FileSystem::GetTablePath("avatar_npc.1.tsv"));
static utils::tsv s_AvatarBBTable(FileSystem::GetTablePath("avatar_bb.1.tsv"));

uint32_t Game::GetActionWasID(int type, int roleID, int actionID)
{
	if (actionID < 0) return -1;
	if (roleID < 0) return -1;
	
	auto* rowTable = &s_AvatarRoleTable;
	switch (type)
	{
	case ACTOR_TYPE_DEFAULT:
		break;
	case ACTOR_TYPE_PLAYER:
		break;
	case ACTOR_TYPE_PET:
		rowTable = &s_AvatarBBTable;
		break;
	case ACTOR_TYPE_NPC:
		rowTable = &s_AvatarNpcTable;
		break;
	default:
		break;
	}
	std::string wasIDstr("");
	if (actionID == Action::Idle || actionID == Action::Batidle)
	{
		auto wasIdle = rowTable->Rows[roleID][s_ActionSet[Action::Idle]];
		auto wasBatidle = rowTable->Rows[roleID][s_ActionSet[Action::Batidle]];
		if (wasIdle != "")wasIDstr = wasIdle;
		else wasIDstr = wasBatidle;
	}
	else
	{
		wasIDstr = rowTable->Rows[roleID][s_ActionSet[actionID]];
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
	if (actionID == Action::Idle || actionID == Action::Batidle)
	{
		auto wasIdle = s_AvatarWeapon90Table.Rows[weaponID][s_ActionSet[Action::Idle]];
		auto wasBatidle = s_AvatarWeapon90Table.Rows[weaponID][s_ActionSet[Action::Batidle]];
		if (wasIdle != "")wasIDstr = wasIdle;
		else wasIDstr = wasBatidle;
	}
	else
	{
		wasIDstr = s_AvatarWeapon90Table.Rows[weaponID][s_ActionSet[actionID]];
	}
	if (wasIDstr == "")return-1;
	uint32 wasID = std::stoul(wasIDstr, 0, 16);
	return wasID;
}

int Game::GetRoleIDByName(int actorType, const char* templ_name)
{
	auto* rowTable = &s_AvatarRoleTable;
	switch (actorType)
	{
	case ACTOR_TYPE_DEFAULT:
		break;
	case ACTOR_TYPE_PLAYER:
		break;
	case ACTOR_TYPE_PET:
		rowTable = &s_AvatarBBTable;
		break;
	case ACTOR_TYPE_NPC:
		rowTable = &s_AvatarNpcTable;
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
