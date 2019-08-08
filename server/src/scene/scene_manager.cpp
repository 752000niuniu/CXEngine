#include "scene_manager.h"
#include "cxrandom.h"
#include "script_system.h"
#include "profile.h"
#include "scene.h"
#include "game.h"
#include "cxmath.h"
#include "file_system.h"
#include "window.h"
#include "logger.h"
#include "actor/actor_manager.h"


static bool s_DrawMask, s_DrawStrider, s_DrawCell, s_DrawMap, s_DrawAnnouncement, s_AutoRun;
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
	script_system_call_function(script_system_get_luastate(), "on_scene_manager_init");

	
};

void SceneManager::SwitchScene(String name)
{
	if (m_pCurrentScene != nullptr)
	{
		if (m_pCurrentScene->GetName() == name)return;
	}
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


bool SceneManager::IsDrawMask()
{
	return s_DrawMask;
}

bool SceneManager::IsDrawStrider()
{
	return s_DrawStrider;
}

bool SceneManager::IsDrawCell()
{
	return s_DrawCell;
}

bool SceneManager::IsDrawMap()
{
	return s_DrawMap;
}

bool SceneManager::IsDrawAnnounce()
{
	return s_DrawAnnouncement;
}

bool SceneManager::IsAutoRun()
{
	return s_AutoRun;
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

			script_system_call_function(script_system_get_luastate(), "on_scene_manager_init_scene", m_pCurrentScene->GetName());

			//m_pCurrentScene->SetPlayerByIndex(0);
			Player* player = actor_manager_fetch_local_player();
			if (player)
			{

				if (m_PlayerEnterX != 0 && m_PlayerEnterY != 0)
				{
					player->SetPos((float)m_PlayerEnterX, (float)m_PlayerEnterY);
				}
			}
		}
		m_SwitchingScene = false;
	}
	else
	{
		if (m_pCurrentScene)
		{
			m_pCurrentScene->Update();
			script_system_call_function(script_system_get_luastate(), "on_scene_manager_update", m_pCurrentScene->GetName());
		}
	}
};

void SceneManager::Draw()
{
	
};

BaseScene* SceneManager::GetCurrentScene()
{
	return m_pCurrentScene;
};


Player* scene_find_player(const char* player_name)
{
	return actor_manager_find_player_by_name(player_name);
}

void scene_set_player(const char* player_name)
{

}

void scene_add_player(const char* player_name, int x, int y, int dir, int role_id, int weapon_id)
{

}


void scene_add_npc(const char* player_name, int  x, int  y, int dir, int role_id, int action_id, const char* msg)
{

}

void scene_add_pet(const char* player_name, int  x, int  y, int dir, int role_id, int action_id)
{

}

void scene_add_player_by_templ_name(const char* templ_name, int actorType)
{

}

void scene_manager_init()
{
	SCENE_MANAGER_INSTANCE->Init();

}

void scene_manager_update()
{
	SCENE_MANAGER_INSTANCE->Update();
}

void scene_manager_draw()
{
	SCENE_MANAGER_INSTANCE->Draw();
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

void scene_manager_switch_scene_by_name(const char* name)
{
	SCENE_MANAGER_INSTANCE->SwitchScene(name);
}

void scene_manager_switch_scene_by_id(int id)
{
	auto& scenes = SCENE_MANAGER_INSTANCE->GetAllScene();
	for (auto& it : scenes) {
		if (it.second->GetSceneID() == id) {
			SCENE_MANAGER_INSTANCE->SwitchScene(it.second->GetName());
			return;
		}
	}
}

void scene_manager_add_scene(int id, const char* name)
{
	SCENE_MANAGER_INSTANCE->AddScene(new Scene(id, name));
}

void scene_manager_add_custom_scene(int id, const char* name)
{
	
}



void scene_manager_set_player_by_index(int index) {

}

void scene_manager_sync_draw_cbx(bool draw_map, bool draw_cell, bool draw_strider, bool draw_mask, bool draw_announcement, bool auto_run) {
	s_DrawMap = draw_map;
	s_DrawCell = draw_cell;
	s_DrawStrider = draw_strider;
	s_DrawMask = draw_mask;
	s_DrawAnnouncement = draw_announcement;
	s_AutoRun = auto_run;
}

BaseScene* scene_manager_fetch_scene(int sceneID)
{
	auto& scenes = SCENE_MANAGER_INSTANCE->GetAllScene();
	for (auto& it : scenes) {
		if (it.second->GetSceneID() == sceneID) {
			return it.second;
		}
	}
	return nullptr;
}

void luaopen_scene_manager(lua_State* L)
{
	script_system_register_function(L, scene_manager_init);
	script_system_register_function(L, scene_manager_update);
	script_system_register_function(L, scene_manager_draw);
	script_system_register_function(L, scene_manager_deinit);
	script_system_register_function(L, scene_manager_add_scene);
	script_system_register_function(L, scene_manager_add_custom_scene);



	script_system_register_function(L, scene_manager_switch_scene_by_id);
	script_system_register_function(L, scene_manager_switch_scene_by_name);

	script_system_register_function(L, scene_manager_set_player_by_index);

	script_system_register_function(L, scene_set_player);
	script_system_register_function(L, scene_add_player);
	script_system_register_function(L, scene_add_npc);
	script_system_register_function(L, scene_add_pet);

	script_system_register_function(L, scene_add_player_by_templ_name);

	script_system_register_function(L, scene_set_announcement);
	script_system_register_function(L, scene_set_chat);

	script_system_register_function(L, scene_manager_sync_draw_cbx);

}