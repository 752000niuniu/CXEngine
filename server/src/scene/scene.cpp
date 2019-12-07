#include "scene.h"

#include "logger.h"
#include "cxrandom.h"
#include "combat/combat.h"
#include <script_system.h>
#include "profile.h"
#include "scene_manager.h"
#include "cxmath.h"
#include "actor/action.h"
#include "combat/Skill.h"
#include <algorithm>
#include <script_system.h>
#include "utils.h" 
#include "window.h"

bool s_IsCombat = true;

Scene::Scene(int id, String name)
	:BaseScene(id, name)
{
	m_SmapWas = -1;
	m_Switching = false;
}

Scene::~Scene()
{
	//SafeDelete(m_SmapPic);
}

void Scene::ApplyTemplate(const std::map<String, String>& templ)
{
	m_SmapWas = utils::tsv_parse_wasstr(templ.at("smap"));
	if (m_SmapWas)
	{
	}
	m_TransportUUIDs = utils::split_by_cuts(templ.at("transport_stations"), ',');

}

void Scene::OnLoad()
{
	//m_LocalPlayer = nullptr;
	m_Switching = true;


	m_ShowSmap = false;


}

void Scene::OnUnLoad()
{
	
}


void Scene::OnSmapClick(float x, float y, float w, float h)
{
	
}

void Scene::Update()
{
	
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
		COMBAT_SYSTEM_INSTANCE->Update();
	}
}

BattleScene::BattleScene(int sceneID, String sceneName)
	:Scene(sceneID, sceneName)
{
	
}

BattleScene::~BattleScene()
{
}

void BattleScene::OnLoad()
{

}

void BattleScene::OnUnLoad()
{

}

static std::vector<std::string> s_CommandSet = { u8"attack",u8"cast" };;


void BattleScene::Update()
{

}




void luaopen_scene(lua_State* L)
{

}
