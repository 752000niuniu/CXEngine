#include "base_scene.h"
#include "scene/scene.h"
#include "utils.h"
#include "game_map.h"
#include "actor/actor_manager.h"
#include "scene/scene_manager.h"
#include "combat/combat.h"

BaseScene::BaseScene(int id, String name)
:m_Name(name),
m_SceneID(id),
m_Map(nullptr),
m_Loaded(false),
m_IsCombat(false)
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

void BaseScene::Update()
{
	if (m_IsCombat) {
		COMBAT_SYSTEM_INSTANCE->Update();
	}
	else {
		actor_manager_update();
	}
}

void BaseScene::Draw()
{
	if (m_IsCombat) {
		COMBAT_SYSTEM_INSTANCE->Draw();
	}
	else {
		Actor* localPlayer = actor_manager_fetch_local_player();
		if (SCENE_MANAGER_INSTANCE->IsDrawMap() && m_Map)
		{
			if (localPlayer == nullptr)
				m_Map->Draw(0, 0);
			else
				m_Map->Draw(localPlayer->GetX(), localPlayer->GetY());
		}

		if (SCENE_MANAGER_INSTANCE->IsDrawCell() && m_Map)
			m_Map->DrawCell();

		actor_manager_draw();
		if (m_Map && localPlayer) {
			m_Map->DrawMask(localPlayer->GetX(), localPlayer->GetY(), localPlayer->GetY());
		}
	}
}

void BaseScene::SetMapID(int id)
{
	if (m_MapID != id) {
		m_MapID = id;
		UnLoad();
		Load();
	}
}

void BaseScene::Load()
{
	if (m_Loaded)return;

	if (m_MapID > 0)
	{
		m_Map = new GameMap(m_MapID);
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

void BaseScene::SetCombat(bool combat)
{
	m_IsCombat = combat;
}
