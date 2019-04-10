#include "base_scene.h"
#include "Scene.h"
#include "Game.h"
#include "utils.h"

BaseScene::BaseScene(int id, String name)
:m_Name(name),
m_SceneID(id),
m_Map(nullptr),
m_LocalPlayer(nullptr),
m_Loaded(false)
{
	
}

BaseScene::~BaseScene()
{
	if (m_Loaded)
	{
		SafeDelete(m_Map);
		m_LocalPlayer = nullptr;
		for (auto* player : m_Players)
		{
			SafeDelete(player);
		}
		m_Players.clear();
		m_Loaded = false;
	}
}

Player* BaseScene::GetPlayerByNickname(const char* nickname)
{
	for (auto* player: m_Players)
	{
		if (player->GetNickName() == nickname)
		{
			return player;
		}
	}
	return nullptr;
}

void BaseScene::Load()
{
	if (m_Loaded)return;

	if (m_SceneID > 0)
	{
		m_Map = new GameMap(m_SceneID);
	}
	else
	{
		m_Map = nullptr;
	}
	OnLoad();

	m_Loaded = true;
}

void BaseScene::UnLoad()
{
	if (m_Loaded)
	{
		OnUnLoad();

		SafeDelete(m_Map);

		//m_LocalPlayer = nullptr;

		for (auto* player : m_Players)
		{
			SafeDelete(player);
		}
		m_Players.clear();
		m_LocalPlayer = nullptr;
		m_Loaded = false;
	}
}

void BaseScene::AddPlayer(const char* player_name, float x, float y,int dir, int role_id, int weapon_id)
{
	Player* player = new Player(role_id);
	player->SetWeaponID(weapon_id);

	//player->LogInfo();
	player->SetPos(x, y);
	player->SetCombatPos(x, y);
	player->SetBox();
	player->SetDir(dir);

	player->SetNickName(player_name);
	player->ChangeAction(Action::Idle);

	m_Players.push_back(player);
}


void BaseScene::AddPlayer(Player* player)
{
	m_Players.push_back(player);
}

void BaseScene::ClearAllPlayers()
{
	m_LocalPlayer = nullptr;

	for (auto* player : m_Players)
	{
		SafeDelete(player);
	}
	m_Players.clear();
}

void BaseScene::AddNpc(const char* player_name, float x, float y, int dir, int role_id, int action_id,const char* msg)
{
	Npc* npc = new Npc(player_name, x, y, dir, role_id, action_id, msg);
	m_Players.push_back(npc);
}

void BaseScene::AddPet(const char* player_name, int x, int y, int dir, int role_id, int action_id)
{
	Pet* pet = new Pet(player_name, (float)x, (float)y, dir, role_id, action_id);
	m_Players.push_back(pet);
}

void BaseScene::SetPlayerByName(const char* player_name)
{
	for (auto* player : m_Players)
	{
		if (player->GetNickName() == std::string(player_name))
		{
			m_LocalPlayer = player;
			m_LocalPlayer->SetAutoRun(false);
			break;
		}
	}	
}

void BaseScene::SetPlayerByIndex(int index)
{
	if (index < 0 || index >= m_Players.size())
	{
		if (m_Players.size() > 0)
		{
			m_LocalPlayer = m_Players[0];
		}
		else
		{
			m_LocalPlayer = nullptr;
		}
	}
	else
	{
		m_LocalPlayer = m_Players[index];
		m_LocalPlayer->SetAutoRun(false);
	}
}

