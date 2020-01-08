#pragma once

#include <define_types.h>
#include <tsv.h>
#include "scene/scene.h"

struct TransportStation
{
	String uuid;
	String to_uuid;
	int cx;
	int cy;
	int width;
	int height;
	String pack;
	uint32_t was;
};

class SceneManager : public Singleton<SceneManager>
{
public:
	SceneManager();

	~SceneManager();

	void Init();

	void Update();

	void Draw();

	BaseScene* GetCurrentScene();

	void SwitchScene(String name);
	
	void SwitchScene(int id);

	void SwitchSceneByTransportUUID(String uuid);
	
	void AddScene(BaseScene* scene);
	
	BaseScene* GetScene(String name) { return m_Scenes.count(name)>0 ? m_Scenes[name] : nullptr; }

	void RemoveScene(String name);
	
	std::map<String, BaseScene*>& GetAllScene() { return m_Scenes; };
	
	bool IsDrawMask();
	bool IsDrawStrider();
	bool IsDrawCell();
	bool IsDrawMap();
	bool IsDrawAnnounce();
	bool IsAutoRun();

	unsigned int GetFboID() { return m_Fbo; };
	unsigned int GetRboID() { return m_Rbo; };
	unsigned int GetTextureID() { return m_TextureColor; };
	Pos GetImGuiCursorPos() { return m_ImGuiCursorPos; }
	TransportStation* GetTransportStationInfo(String uuid);
private:	
	std::map<String,BaseScene*> m_Scenes;

	BaseScene* m_pCurrentScene;

	BaseScene* m_pNextScene;

	utils::tsv m_MapTSV;

	utils::tsv m_TransportStationsTSV;

	std::map<String,TransportStation> m_TransportStations;

	String m_PlayerName;
	int m_PlayerEnterX;
	int m_PlayerEnterY;
	bool m_SwitchingScene;

	unsigned int m_Fbo;
	unsigned int m_Rbo;
	unsigned int m_TextureColor;
	Pos m_ImGuiCursorPos;
};

#define SCENE_MANAGER_INSTANCE SceneManager::GetInstance()

Player* scene_find_player(const char* player_name);
void scene_set_player(const char* player_name);
void scene_add_player_by_templ_name(const char* templ_name, int actorType);
void scene_add_player(const char* player_name, int x, int y, int dir, int role_id, int weapon_id);

bool scene_is_combat();
BaseScene*  scene_manager_fetch_scene(int sceneID);
void luaopen_scene_manager(lua_State* L);
