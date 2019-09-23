#include "scene_manager.h"
#include "cxrandom.h"
#include "scene/was_viewer_scene.h"
#include "scene/ui_scene.h"
#include "script_system.h"
#include "profile.h"
#include "scene.h"
#include "input_manager.h"
#include "game.h"
#include "cxmath.h"
#include "file_system.h"
#include "window.h"
#include "net.h"
#include "logger.h"
#include "actor/actor_manager.h"
#include "animation/sprite.h"
#include "cxlua.h"
#include "graphics/ui_renderer.h"


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
	UIRenderer::GetInstance()->DeleteSingleton();
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
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	UIRenderer::GetInstance();
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
			
			script_system_call_function(script_system_get_luastate(), "on_scene_manager_init_scene" ,m_pCurrentScene->GetName());
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
		ImGui::Begin("Game",0, ImGuiWindowFlags_NoBackground);
		ImGui::BeginChild("##main", ImVec2((float)gameWidth, (float)gameHeight), false, ImGuiWindowFlags_NoBackground);
		glBindFramebuffer(GL_FRAMEBUFFER, SCENE_MANAGER_INSTANCE->GetFboID());
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glViewport(0, 0, gameWidth, gameHeight);
		m_pCurrentScene->Draw();
		script_system_call_function(script_system_get_luastate(), "on_scene_manager_draw", m_pCurrentScene->GetName());
		UIRenderer::GetInstance()->Draw();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		ImVec2 cursorPos = ImGui::GetCursorPos();
		auto cspos = ImGui::GetCursorScreenPos();
		m_ImGuiCursorPos = Pos(cspos.x, cspos.y);
		ImGui::GetWindowDrawList()->AddCallback(function_to_select_shader_or_blend_state, nullptr);
		ImGui::Image((void*)(uint64_t)m_TextureColor, ImVec2((float)gameWidth, (float)gameHeight), ImVec2(0, 1), ImVec2(1, 0));
		ImGui::GetWindowDrawList()->AddCallback(function_to_restore_shader_or_blend_state , nullptr);
		ImGui::SetCursorPos(cursorPos);

		script_system_call_function(script_system_get_luastate(), "on_game_imgui_update", m_pCurrentScene->GetName());

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
	return actor_manager_find_player_by_name(player_name);
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
	auto& scenes=  SCENE_MANAGER_INSTANCE->GetAllScene();
	for(auto& it :scenes){
		if(it.second->GetSceneID()==id){
			SCENE_MANAGER_INSTANCE->SwitchScene(it.second->GetName());
			return;
		}
	}
}

void scene_manager_add_scene(int id , const char* name)
{
	SCENE_MANAGER_INSTANCE->AddScene(new Scene( id , name ));
}

void scene_manager_add_custom_scene(int id, const char* name)
{
	if (strcmp(name, "WASViewer") == 0) {
		SCENE_MANAGER_INSTANCE->AddScene(new WASViewerScene(id, name));
	}
	else if (strcmp(name, "UIScene") == 0) {
		SCENE_MANAGER_INSTANCE->AddScene(new UIScene(id, name));
	}
	else {
		SCENE_MANAGER_INSTANCE->AddScene(new BaseScene(id, name));
	}
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

int scene_manager_get_imgui_cursor_pos(lua_State* L) {
	Pos pos = SCENE_MANAGER_INSTANCE->GetImGuiCursorPos();
	lua_pushnumber(L, pos.x);
	lua_pushnumber(L, pos.y);
	return 2;
};

int scene_manager_get_current_scene_id(){
	auto* scene = SCENE_MANAGER_INSTANCE->GetCurrentScene();
	if(scene){
		return scene->GetSceneID();
	}
	return 0;
}

bool scene_is_combat()
{
	auto* scene = SCENE_MANAGER_INSTANCE->GetCurrentScene();
	if (scene) {
		return scene->IsCombat();
	}
	return false;
}

void scene_set_map(int mapid)
{
	auto* scene = SCENE_MANAGER_INSTANCE->GetCurrentScene();
	if (scene) {
		scene->SetMapID(mapid);
	}
}

void luaopen_scene_manager(lua_State* L)
{
	script_system_register_function(L, scene_manager_init);
	script_system_register_function(L, scene_manager_update);
	script_system_register_function(L, scene_manager_draw);
	script_system_register_function(L, scene_manager_deinit);
	script_system_register_function(L, scene_manager_add_scene);
	script_system_register_function(L, scene_manager_add_custom_scene);

	script_system_register_function(L, scene_is_combat);
	script_system_register_function(L, scene_set_map);

	script_system_register_luac_function(L, scene_manager_get_imgui_cursor_pos);

	
	script_system_register_function(L, scene_manager_switch_scene_by_id);
	script_system_register_function(L, scene_manager_switch_scene_by_name);


	script_system_register_function(L, scene_set_announcement);
	script_system_register_function(L, scene_set_chat);

	script_system_register_function(L, scene_manager_get_current_scene_id);

	script_system_register_function(L, scene_manager_sync_draw_cbx);

}