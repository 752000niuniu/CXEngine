#pragma once
#include "defines.h"
#include "defineTypes.h"
#include "tsv.h"

/*
负责场景切换，添加 移除 等等
*/
class Scene;
class BaseScene;

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


	BaseScene* GetCurrentScene();

	void SwitchScene(String name);
	
	void SwitchScene(int id);

	void SwitchSceneByTransportUUID(String uuid);
	
	void AddScene(BaseScene* scene);
	
	BaseScene* GetScene(String name) { return m_Scenes.count(name)>0 ? m_Scenes[name] : nullptr; }

	void RemoveScene(String name);
	
	std::map<String, BaseScene*>& GetAllScene() { return m_Scenes; };
	

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
};

#define SCENE_MANAGER_INSTANCE SceneManager::GetInstance()


void luaopen_scene_manager(lua_State* L);
