#include "scene_manager.h"
#include "cxrandom.h"
#include "splash_scene.h"
#include "was_viewer_scene.h"
#include "ui_scene.h"
#include "script_system.h"
#include "profile.h"
#include "test_scene.h"
#include "Scene.h"
#include "input_manager.h"
#include "game.h"
#include "cxmath.h"
#include "file_system.h"
#include "window.h"
#include "net.h"
#include "../imgui_internal.h"


static bool s_DrawMask, s_DrawStrider, s_DrawCell, s_DrawMap, s_DrawAnnouncement, s_AutoRun;
static bool g_IsMouseInImGui;
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

	glGenFramebuffers(1, &m_Fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_Fbo);

	int screenWidth = WINDOW_INSTANCE->GetWidth(); 
	int screenHeight = WINDOW_INSTANCE->GetHeight();
	glGenTextures(1, &m_TextureColor);
	glBindTexture(GL_TEXTURE_2D, m_TextureColor);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screenWidth, screenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_TextureColor, 0);

	glGenRenderbuffers(1, &m_Rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, m_Rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_Rbo); 
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		cxlog_err("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
	return	g_IsMouseInImGui;
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
			
			script_system_call_function(script_system_get_luastate(), "on_scene_manager_init_scene" ,m_pCurrentScene->GetName());

			//m_pCurrentScene->SetPlayerByIndex(0);
			if (m_pCurrentScene->GetLocalPlayer())
			{
				if (m_PlayerEnterX != 0 && m_PlayerEnterY != 0)
				{
					m_pCurrentScene->GetLocalPlayer()->SetPos((float)m_PlayerEnterX, (float)m_PlayerEnterY);
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
			script_system_call_function(script_system_get_luastate(),"on_scene_manager_update", m_pCurrentScene->GetName());
		}
	} 
	auto mouseX = INPUT_MANAGER_INSTANCE->GetMouseX();
	auto mouseY = INPUT_MANAGER_INSTANCE->GetMouseY();
	ImVec2 pos = ImGui::GetWindowPos();
	ImVec2 size = ImGui::GetWindowSize();
	Bound bound{ pos.x, pos.x + size.x, pos.y, pos.y + size.y };
	g_IsMouseInImGui = utils::BoundHitTest(bound, Pos{ mouseX, mouseY });
};

void function_to_select_shader_or_blend_state(const ImDrawList* parent_list, const ImDrawCmd* cmd){
	glDisable(GL_BLEND);
}

void function_to_restore_shader_or_blend_state(const ImDrawList* parent_list, const ImDrawCmd* cmd){
	glEnable(GL_BLEND);
}

void SceneManager::Draw() 
{
	if (m_SwitchingScene)return;
	if(m_pCurrentScene)
	{
		int gameWidth = WINDOW_INSTANCE->GetWidth();
		int gameHeight = WINDOW_INSTANCE->GetHeight();
		
		ImGui::Begin("Game");
		ImGui::BeginChild("##main", ImVec2((float)gameWidth, (float)gameHeight));
		glBindFramebuffer(GL_FRAMEBUFFER, SCENE_MANAGER_INSTANCE->GetFboID());
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glViewport(0, 0, gameWidth, gameHeight);
		m_pCurrentScene->Draw();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		ImVec2 cursorPos = ImGui::GetCursorPos();
		ImGui::GetWindowDrawList()->AddCallback(function_to_select_shader_or_blend_state, nullptr);
		ImGui::Image((void*)(uint64_t)m_TextureColor, ImVec2((float)gameWidth, (float)gameHeight), ImVec2(0, 1), ImVec2(1, 0));
		ImGui::GetWindowDrawList()->AddCallback(function_to_restore_shader_or_blend_state , nullptr);
		ImGui::SetCursorPos(cursorPos);

		script_system_call_function(script_system_get_luastate(), "on_game_imgui_update");

		auto* player = m_pCurrentScene->GetLocalPlayer();
		if (player) {
			if (ImGui::IsMouseClicked(0)) {
				ImVec2 mpos = ImGui::GetMousePos();
				ImVec2 wpos = ImGui::GetWindowPos();
				mpos.x = mpos.x - wpos.x;
				mpos.y = mpos.y - wpos.y ;
				Pos dest = GAME_INSTANCE->ScreenPosToMapPos({ mpos.x, mpos.y });
				player->MoveTo(m_pCurrentScene->GetGameMap(), (int)dest.x, (int)dest.y);
				net_send_move_to_pos_message(player->GetNickName(), dest.x, dest.y);
			}
		}
		ImGui::EndChild();
		ImGui::End();
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
		scene->AddNpc(player_name, (float)x, (float)y,dir, role_id, action_id, msg);
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

void scene_manager_add_scene(int id , const char* name)
{
	SCENE_MANAGER_INSTANCE->AddScene(new Scene( id , name ));
}

void scene_manager_add_custom_scene(int id, const char* name)
{
	if (strcmp(name, "BattleScene") == 0) {
		SCENE_MANAGER_INSTANCE->AddScene(new BattleScene(id, name));
	}
	else if (strcmp(name, "Splash") == 0) {
		SCENE_MANAGER_INSTANCE->AddScene(new SplashScene(id, name));
	}
	else if (strcmp(name, "WASViewer") == 0) {
		SCENE_MANAGER_INSTANCE->AddScene(new WASViewerScene(id, name));
	}
	else if (strcmp(name, "UIScene") == 0) {
		SCENE_MANAGER_INSTANCE->AddScene(new UIScene(id, name));
	}
	else if (strcmp(name, "TestScene") == 0) {
		SCENE_MANAGER_INSTANCE->AddScene(new TestScene(id, name));
	}
	else if (strcmp(name, "TestNetScene") == 0) {
		SCENE_MANAGER_INSTANCE->AddScene(new TestNetScene(id, name));
	}
	else if (strcmp(name, "AnimationEditor") == 0) {
		SCENE_MANAGER_INSTANCE->AddScene(new AnimationEditorScene(id, name));
	}
}


int scene_manager_fetch_local_player(lua_State* L){
	auto* scene = SCENE_MANAGER_INSTANCE->GetCurrentScene();
	if (scene == nullptr) return 0;
	auto* player = scene->GetLocalPlayer();
	if (player == nullptr)return 0;
	lua_push_actor(L, player);
	return 1;
}

void scene_manager_set_player_by_index(int index) {
	auto* scene = SCENE_MANAGER_INSTANCE->GetCurrentScene();
	if (scene == nullptr) return;
	scene->SetPlayerByIndex(index);
}

void scene_manager_sync_draw_cbx(bool draw_map, bool draw_cell, bool draw_strider, bool draw_mask, bool draw_announcement, bool auto_run) {
	s_DrawMap = draw_map;
	s_DrawCell = draw_cell;
	s_DrawStrider = draw_strider;
	s_DrawMask = draw_mask;
	s_DrawAnnouncement = draw_announcement;
	s_AutoRun = auto_run;
}

void luaopen_scene_manager(lua_State* L)
{
	script_system_register_function(L, scene_manager_init);
	script_system_register_function(L, scene_manager_update);
	script_system_register_function(L, scene_manager_draw);
	script_system_register_function(L, scene_manager_deinit);
	script_system_register_function(L, scene_manager_add_scene);
	script_system_register_function(L, scene_manager_add_custom_scene);


	script_system_register_luac_function(L, scene_manager_fetch_local_player);

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