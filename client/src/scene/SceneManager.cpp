#include "SceneManager.h"
#include "../Random.h"
#include "SplashScene.h"
#include "WASViewerScene.h"
#include "UIScene.h"
#include "script_system.h"
#include "profile.h"
#include "test_scene.h"
#include "Scene.h"
#include "InputManager.h"
#include "Game.h"
#include "GMath.h"

static bool s_DrawMask(true), s_DrawStrider(true), s_DrawCell(false), s_DrawMap(true), s_DrawAnnouncement(true), s_AutoRun(false);
bool g_IsMouseInImGui = false;

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
	AddScene(new SplashScene(-100, "Splash"));
	AddScene(new WASViewerScene(-101, "WASViewer"));
	AddScene(new UIScene(-102, "UIScene"));
	AddScene(new TestScene(-103, "TestScene"));
	AddScene(new TestNetScene(-104, "TestNetScene"));
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
	script_system_call_function("on_scene_manager_init");
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

bool SceneManager::IsHoverImGui()
{
	return g_IsMouseInImGui;
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
			//m_pCurrentScene->SetPlayerByIndex(0);
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
			script_system_call_function("on_scene_manager_update");
			m_pCurrentScene->Update();
		}
	}
};

void SceneManager::DrawImGUI()
{
	if (m_SwitchingScene)return;
	auto* curScene = GetCurrentScene();
	if (!curScene)return;
	auto m_Player = curScene->GetLocalPlayer();
	if (!m_Player)return;
	ImGui::Begin("[Scene]", NULL, ImGuiWindowFlags_NoTitleBar);

	Pos mousePos = INPUT_MANAGER_INSTANCE->GetMousePos();
	Pos mouseWorldPos = GAME_INSTANCE->ScreenPosToMapPos(mousePos);
	ImGui::Text("[mouse] : x=%.0f,y=%.0f world.x=%.0f,world.y=%.0f", 
		mousePos.x, mousePos.y,
		mouseWorldPos.x, mouseWorldPos.y);
	ImGui::Text("[player] : x=%d,y=%d,dir=%d", m_Player->GetX(), m_Player->GetY(), m_Player->GetDir());
	ImGui::SameLine();
	ImGui::Checkbox("DrawMap", &s_DrawMap);
	ImGui::SameLine();
	ImGui::Checkbox("DrawCell", &s_DrawCell);
	ImGui::SameLine();
	ImGui::Checkbox("DrawStrider", &s_DrawStrider);
	ImGui::SameLine();
	ImGui::Checkbox("DrawMask", &s_DrawMask);
	ImGui::SameLine();
	ImGui::Checkbox("DrawAnnouncement", &s_DrawAnnouncement);
	ImGui::SameLine();
	ImGui::Checkbox("AutoRun", &s_AutoRun);

	static int s_SceneNameIndex = 0;
	static std::vector<std::string> m_SceneNames;
	if (ImGui::CollapsingHeader("CHSceneList"))
	{
		auto& allScene = SCENE_MANAGER_INSTANCE->GetAllScene();
		if (m_SceneNames.size() == 0)
		{
			for (auto& scene : allScene)
			{
				m_SceneNames.push_back(scene.second->GetName().c_str());
			}
		}
		ImGui::ListBox("##SceneList", &s_SceneNameIndex, [](void* data, int idx, const char** out_text)
		{
			std::vector<std::string>* str_vec = (std::vector<std::string> *)data;
			*out_text = (*str_vec)[idx].c_str();
			return true;
		}, (void*)&m_SceneNames, (int)m_SceneNames.size(), 20);
		ImGui::SameLine();
		if (ImGui::Button("SwitchSceneByName"))
		{
			SCENE_MANAGER_INSTANCE->SwitchScene(m_SceneNames[s_SceneNameIndex]);
		}
	}

	for (int i = 0; i < 10; i++)
	{
		std::string name("Player");
		name.append(std::to_string(i));
		if (ImGui::Button(name.c_str()))
		{
			curScene->SetPlayerByIndex(i);
		}
		ImGui::SameLine();
		if (i == 9)
		{
			if (ImGui::Button("Clear"))
			{
				curScene->ClearAllPlayers();
				curScene->AddPlayer("test", 520, 1050, 1, 9, 5);
				curScene->SetPlayerByIndex(0);
			}
		}
	}
	

	static char s_templ_name[128];
	static int s_copy_actor_type = ACTOR_TYPE_PLAYER;
	
	ImGui::Text(u8"TemplName  :");
	ImGui::SameLine();
	ImGui::InputText("##templ_name", s_templ_name, 128);
	if (ImGui::Button("召唤Avatar"))
	{
		scene_add_player_by_templ_name(s_templ_name, s_copy_actor_type);
	}
	ImGui::SameLine();
	ImGui::SliderInt("##actor_type", &s_copy_actor_type,1,3);
	

	if (ImGui::CollapsingHeader("PlayAction"))
	{
		for (size_t i = 0; i < s_ActionSet.size(); i++)
		{
			if (ImGui::Button(s_ActionSet[i].c_str()))
			{
				if (m_Player != nullptr)
				{
					m_Player->SetActionID(i);
				}
			}
			if (i != s_ActionSet.size() - 1) ImGui::SameLine();
		}
	}

	if (ImGui::Button("SwitchToLogin"))
	{
		SCENE_MANAGER_INSTANCE->SwitchScene("TestNetScene");
	}

	if (ImGui::Button("SwitchToBattle"))
	{
		SCENE_MANAGER_INSTANCE->SwitchScene("BattleScene");
	}

	auto mouseX = INPUT_MANAGER_INSTANCE->GetMouseX();
	auto mouseY = INPUT_MANAGER_INSTANCE->GetMouseY();
	ImVec2 pos = ImGui::GetWindowPos();
	ImVec2 size = ImGui::GetWindowSize();
	Bound bound{ pos.x, pos.x + size.x, pos.y, pos.y + size.y };
	g_IsMouseInImGui = utils::BoundHitTest(bound, Pos{ mouseX, mouseY });
	ImGui::End();
}


void SceneManager::Draw() 
{
	if (m_SwitchingScene)return;
	if(m_pCurrentScene)
	{
		script_system_call_function("on_scene_manager_draw");
		m_pCurrentScene->Draw();
		DrawImGUI();
	}
};

BaseScene* SceneManager::GetCurrentScene()
{
	return m_pCurrentScene;
};


Player* scene_find_player(const char* player_name)
{
	BaseScene* scene = SCENE_MANAGER_INSTANCE->GetCurrentScene();
	if (scene)
	{
		return scene->GetPlayerByNickname(player_name);
	}
	else 
		return nullptr;
}

void scene_set_player(const char* player_name)
{
	BaseScene* scene = SCENE_MANAGER_INSTANCE->GetCurrentScene();
	if(scene)
	{
		scene->SetPlayerByName(player_name);
	}
}

void scene_add_player(const char* player_name, int x, int y, int dir,int role_id, int weapon_id)
{
	BaseScene* scene = SCENE_MANAGER_INSTANCE->GetCurrentScene();
	if (scene)
	{
		scene->AddPlayer(player_name, (float)x, (float)y, dir, role_id, weapon_id);
	}
}

void scene_add_npc(const char* player_name, int  x, int  y, int dir, int role_id, int action_id, const char* msg)
{
	BaseScene* scene = SCENE_MANAGER_INSTANCE->GetCurrentScene();
	if (scene)
	{
		scene->AddNpc(player_name, x, y,dir, role_id, action_id, msg);
	}
}

void scene_add_pet(const char* player_name, int  x, int  y, int dir, int role_id, int action_id)
{
	BaseScene* scene = SCENE_MANAGER_INSTANCE->GetCurrentScene();
	if (scene)
	{
		scene->AddPet(player_name, x, y, dir, role_id, action_id);
	}
}

void scene_add_player_by_templ_name(const char* templ_name, int actorType)
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
	const char* defaultSceneName = script_system_get_globle<const char* >("DefaultSceneName");
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
void luaopen_scene_manager(lua_State* L)
{
	script_system_register_function(L, scene_manager_init);
	script_system_register_function(L, scene_manager_update);
	script_system_register_function(L, scene_manager_draw);
	script_system_register_function(L, scene_manager_deinit);

	script_system_register_function(L, scene_manager_switch_scene_by_name);

	script_system_register_function(L, scene_set_player);
	script_system_register_function(L, scene_add_player);
	script_system_register_function(L, scene_add_npc);
	script_system_register_function(L, scene_add_pet);

	script_system_register_function(L, scene_add_player_by_templ_name);

	script_system_register_function(L, scene_set_announcement);
	script_system_register_function(L, scene_set_chat);

}