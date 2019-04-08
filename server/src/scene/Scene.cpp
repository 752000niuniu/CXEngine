#include "Scene.h"

#include "Logger.h"
#include "cxrandom.h"
#include "script_system.h"
#include "scene_manager.h"
#include "cxmath.h"
#include "actor/action.h"
#include <algorithm>
#include "Game.h"

bool s_IsCombat = true;

Scene::Scene(int id,String name)
:BaseScene(id,name)
{
	m_SmapWas = -1;
	m_Switching = false;
}

Scene::~Scene()
{   
}

void Scene::ApplyTemplate(const std::map<String, String>& templ)
{
	m_SmapWas = utils::tsv_parse_wasstr(templ.at("smap"));
	m_TransportUUIDs = utils::split_by_cuts(templ.at("transport_stations"), ',');
}

void Scene::OnLoad()
{
	m_LocalPlayer = nullptr;
	m_Switching = true;

	for (auto id : m_TransportUUIDs)
	{
		auto* info = SCENE_MANAGER_INSTANCE->GetTransportStationInfo(id);
		if (info != nullptr)
		{
		}
	}
	
	m_ShowSmap = false;

	script_system_call_function(script_system_get_luastate(),"on_scene_init");


}

void Scene::OnUnLoad()
{
}


void Scene::OnSmapClick(float x, float y, float w, float h)
{
	if (m_LocalPlayer)
	{
		x = std::min(w, std::max(0.f, x));
		y = std::min(h, std::max(0.f, y));
		float dx = x / w * m_Map->GetMapWidth();
		float dy = y / h * m_Map->GetMapHeight();
		m_LocalPlayer->MoveTo(m_Map, dx, dy);
	}
}

void Scene::Update()
{
	script_system_call_function(script_system_get_luastate(),"on_scene_update");
	
	float dt = GAME_INSTANCE->GetDeltaTime();
	for (auto* player: m_Players)
	{
		player->OnUpdate(dt);
	}

}


void Scene::SetAnnouncement(const char* text)
{
	
}

void Scene::SetChat(const char* text)
{
	
}

int Scene::GetMapOffsetX()
{
	return m_Map->GetMapOffsetX();
}
int Scene::GetMapOffsetY()
{
	return m_Map->GetMapOffsetY();
}

void Scene::SwitchBattle()
{
	s_IsCombat = !s_IsCombat;
	if (s_IsCombat)
	{
	}
}

BattleScene::BattleScene(int sceneID, String sceneName)
	:Scene(sceneID,sceneName)
{
	m_LocalPlayer = nullptr;
}

BattleScene::~BattleScene()
{
}

void BattleScene::OnLoad()
{
	script_system_call_function(script_system_get_luastate(),"on_battle_scene_init");
}

void BattleScene::OnUnLoad()
{

}

static std::vector<std::string> s_CommandSet = { u8"attack",u8"cast" };;
void BattleScene::Update()
{
	auto* m_Player = GetLocalPlayer();
	
	
}


