#include "SceneManager.h"
#include "cxrandom.h"
#include "script_system.h"
#include "Scene.h"
#include "Game.h"
#include "cxmath.h"
#include "actor/action.h"
#include "file_system.h"
static bool s_DrawMask(true), s_DrawStrider(true), s_DrawCell(false), s_DrawMap(true), s_DrawAnnouncement(true), s_AutoRun(false);
bool g_IsMouseInImGui = false;

static void scene_add_player_by_templ_name(const char* templ_name, int actorType);
/*
lua 尽快完成tsv解析  然后把scene创建放在lua
*/
SceneManager::SceneManager()
:m_pCurrentScene(nullptr),
m_pNextScene(nullptr),
m_MapTSV(utils::tsv(FileSystem::GetTSVPath("map"))),
m_TransportStationsTSV(utils::tsv(FileSystem::GetTSVPath("transport_station"))),
m_PlayerEnterX(0),
m_PlayerEnterY(0),
m_SwitchingScene(false)
{
	m_TransportStations.clear();
	//TODO move this to lua
	for (const auto& row : m_TransportStationsTSV.Rows)
	{
		TransportStation station;
		station.uuid = row.at("uuid");
		station.to_uuid = row.at("to_uuid");
		station.cx = std::stoi(row.at("cx"));
		station.cy = std::stoi(row.at("cy"));
		station.width = std::stoi(row.at("width"));
		station.height = std::stoi(row.at("height"));
		station.pack = row.at("pack");
		station.was = std::stoul(row.at("was"), 0, 16);
		m_TransportStations.insert({ station.uuid, station });
	}

	//TODO move this to lua
	for (const auto& row : m_MapTSV.Rows)
	{
		int id = std::stoi(row.at("ID"));
		String name = row.at("name");
		Scene* scene = new Scene(id, name);
		scene->ApplyTemplate(row);
		AddScene(scene);
	}
	
	AddScene(new BattleScene(-1, "BattleScene"));
}

SceneManager::~SceneManager()
{
	for (auto& scene : m_Scenes)
	{
		delete scene.second;
		scene.second = nullptr;
	}
	m_Scenes.clear();
}

void SceneManager::Init() 
{
	script_system_call_function(script_system_get_luastate(),"on_scene_manager_init");
};

void SceneManager::SwitchScene(String name)
{
	auto it = m_Scenes.find(name);
	if (it != m_Scenes.end())
	{
		m_pNextScene = it->second;
		m_SwitchingScene = true;
	}
}

void SceneManager::SwitchScene(int id)
{
	for (auto& scene : m_Scenes)
	{
		if (scene.second->GetSceneID() == id)
		{
			m_pNextScene = scene.second;
			m_SwitchingScene = true;
			break;
		}
	}
}

void SceneManager::SwitchSceneByTransportUUID(String uuid)
{
	auto* stationInfo = GetTransportStationInfo(uuid);
	if (stationInfo != nullptr)
	{
		auto mapinfo = utils::split_by_cuts(uuid, '_');
		auto mapid = std::stoi(mapinfo[0]);
		m_PlayerEnterX = stationInfo->cx;
		m_PlayerEnterY = stationInfo->cy;
		SwitchScene(mapid);
	}
}

void SceneManager::AddScene(BaseScene* scene)
{
	if (!scene) return;
	if (m_Scenes.count(scene->GetName()) > 0) return;
	m_Scenes[scene->GetName()] = scene;
};

void SceneManager::RemoveScene(String name)
{
	auto it = m_Scenes.find(name);
	if (it != m_Scenes.end())
	{
		delete it->second;
		it->second = nullptr;
		m_Scenes.erase(it);
	}
}

TransportStation* SceneManager::GetTransportStationInfo(String uuid)
{
	auto it = m_TransportStations.find(uuid);
	if (it == m_TransportStations.end())
	{
		return nullptr;
	}
	else
	{
		return &it->second;
	}
}

void SceneManager::Update() 
{
	if (m_SwitchingScene)
	{
		if (m_pNextScene != nullptr)
		{
			if (m_pCurrentScene)
			{
				m_pCurrentScene->UnLoad();
				m_pCurrentScene = nullptr;
			}
			
			m_pCurrentScene = m_pNextScene;
			m_pCurrentScene->Load();
			m_pCurrentScene->SetPlayerByIndex(0);
			if (m_pCurrentScene->GetLocalPlayer())
			{
				if (m_PlayerEnterX != 0 && m_PlayerEnterY != 0)
				{
					m_pCurrentScene->GetLocalPlayer()->SetPos(m_PlayerEnterX, m_PlayerEnterY);
				}
			}
		}
		m_SwitchingScene = false;
	}
	else
	{
		if (m_pCurrentScene)
		{
			script_system_call_function(script_system_get_luastate(),"on_scene_manager_update");
			m_pCurrentScene->Update();
		}
	}
};


BaseScene* SceneManager::GetCurrentScene()
{
	return m_pCurrentScene;
};

static void scene_set_player(const char* player_name)
{
	BaseScene* scene = SCENE_MANAGER_INSTANCE->GetCurrentScene();
	if(scene)
	{
		scene->SetPlayerByName(player_name);
	}
}

static void scene_add_player(const char* player_name, int x, int y, int dir,int role_id, int weapon_id)
{
	BaseScene* scene = SCENE_MANAGER_INSTANCE->GetCurrentScene();
	if (scene)
	{
		scene->AddPlayer(player_name, (float)x, (float)y, dir, role_id, weapon_id);
	}
}

static void scene_add_npc(const char* player_name, int  x, int  y, int dir, int role_id, int action_id, const char* msg)
{
	BaseScene* scene = SCENE_MANAGER_INSTANCE->GetCurrentScene();
	if (scene)
	{
		scene->AddNpc(player_name, x, y,dir, role_id, action_id, msg);
	}
}

static void scene_add_pet(const char* player_name, int  x, int  y, int dir, int role_id, int action_id)
{
	BaseScene* scene = SCENE_MANAGER_INSTANCE->GetCurrentScene();
	if (scene)
	{
		scene->AddPet(player_name, x, y, dir, role_id, action_id);
	}
}

static void scene_add_player_by_templ_name(const char* templ_name, int actorType)
{
	BaseScene* scene = SCENE_MANAGER_INSTANCE->GetCurrentScene();
	if (scene)
	{
		Player* localPlayer = scene->GetLocalPlayer();
		Player* player = nullptr;
		
		int roleID = GAME_INSTANCE->GetRoleIDByName(actorType, templ_name);
		switch (actorType)
		{
		case ACTOR_TYPE_PLAYER:
			player = new Player(roleID);
			break;
		case ACTOR_TYPE_NPC:
			player = new Npc(templ_name, 0, 0, 0, roleID, 0, "");
			break;
		case ACTOR_TYPE_PET:
			player = new Pet(templ_name,0,0,0,roleID,0);
			break;
		default:
			break;
		}
		std::string player_name(templ_name);
		player_name.append("_");
		player_name.append(std::to_string(scene->GetPlayersCount()));
		player->SetNickName(player_name.c_str());
		if (localPlayer != nullptr)
		{
			player->SetPos(localPlayer->GetPos());
			player->SetCombatPos(player->GetPos());
			player->SetBox();
			player->SetDir(localPlayer->GetDir());
			player->SetWeaponID(-1);
			player->ChangeAction(Action::Idle);
		}
		scene->AddPlayer(player);
	}
}

void scene_manager_init()
{
	SCENE_MANAGER_INSTANCE->Init();
	const char* defaultSceneName = script_system_get_globle<const char* >("DefaultSceneName") ;
	if (defaultSceneName)
	{
		SCENE_MANAGER_INSTANCE->SwitchScene(defaultSceneName);
	}
}


void scene_manager_update()
{
	SCENE_MANAGER_INSTANCE->Update();
}


void scene_manager_draw()
{

}

void scene_manager_deinit()
{
	SCENE_MANAGER_INSTANCE->DeleteSingleton();
}

void scene_set_announcement(const char* text)
{
	BaseScene* scene = SCENE_MANAGER_INSTANCE->GetCurrentScene();
	if (scene)
	{
		dynamic_cast<Scene*>(scene)->SetAnnouncement(text);
	}

}

void scene_set_chat(const char* text)
{
	BaseScene* scene = SCENE_MANAGER_INSTANCE->GetCurrentScene();
	if (scene)
	{
		dynamic_cast<Scene*>(scene)->SetChat(text);
	}
}

void luaopen_scene_manager(lua_State* L)
{
	script_system_register_function(L, scene_manager_init);
	script_system_register_function(L, scene_manager_update);
	script_system_register_function(L, scene_manager_draw);
	script_system_register_function(L, scene_manager_deinit);

	script_system_register_function(L, scene_set_player);
	script_system_register_function(L, scene_add_player);
	script_system_register_function(L, scene_add_npc);
	script_system_register_function(L, scene_add_pet);

	script_system_register_function(L, scene_add_player_by_templ_name);

	script_system_register_function(L, scene_set_announcement);
	script_system_register_function(L, scene_set_chat);
}