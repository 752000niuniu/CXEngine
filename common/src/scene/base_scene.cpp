#include "base_scene.h"
#include "scene/scene.h"
#include "game.h"
#include "utils.h"
#include "game_map.h"

BaseScene::BaseScene(int id, String name)
:m_Name(name),
m_SceneID(id),
m_Map(nullptr),
m_Loaded(false)
{
	
}

BaseScene::~BaseScene()
{
	if (m_Loaded)
	{
		SafeDelete(m_Map);
		m_Loaded = false;
	}
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

		m_Loaded = false;
	}
}

