#include "scene.h"

#include "../animation/frame_animation.h"
#include "logger.h"
#include "cxrandom.h"
#include "../combat/combat.h"
#include "script_system.h"
#include "profile.h"
#include "scene_manager.h"
#include "cxmath.h"
#include "actor/action.h"
#include "combat/Skill.h"
#include "resource_manager.h"
#include "actor/player_state.h"
#include <algorithm>
#include "script_system.h"
#include "utils.h"
#include "window.h"

bool s_IsCombat = true;
std::map<String, FrameAnimation*> s_TranportCircles;

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

	for (auto id : m_TransportUUIDs)
	{
		auto* info = SCENE_MANAGER_INSTANCE->GetTransportStationInfo(id);
		if (info != nullptr)
		{
			auto* frame = new FrameAnimation(MapaniWDF, 0x7F4CBC8C);
			frame->SetPos({ info->cx, info->cy });
			s_TranportCircles.insert({ id,frame });
		}
	}

	m_ShowSmap = false;


}

void Scene::OnUnLoad()
{
	for (auto& it : s_TranportCircles)
	{
		SafeDelete(it.second);
	}
	s_TranportCircles.clear();
}


void Scene::OnSmapClick(float x, float y, float w, float h)
{
	if (m_LocalPlayer)
	{
		x = std::min(w, std::max(0.f, x));
		y = std::min(h, std::max(0.f, y));
		float dx = x / w * m_Map->GetMapWidth();
		float dy = y / h * m_Map->GetMapHeight();
		m_LocalPlayer->MoveTo(m_Map, (int)dx, (int)dy);
	}
}

void Scene::Update()
{
	float dt = WINDOW_INSTANCE->GetDeltaTime();

	for (auto& it : s_TranportCircles)
	{
		it.second->OnUpdate();
	}

	for (auto* player : m_Players)
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
	auto* m_Player = GetLocalPlayer();
	if (ImGui::CollapsingHeader("CombatCommand"))
	{
		for (size_t i = 0; i < s_CommandSet.size(); i++)
		{
			std::string& command = s_CommandSet[i];
			if (ImGui::Button(command.c_str()) && m_Player != nullptr)
			{
				if (command == u8"attack")
				{

					auto& self = m_Players[0];
					auto& enemy = m_Players[1];
					if (m_Player != self)
					{
						enemy = self;
						self = m_Player;
					}
					Pos targetPos = enemy->GetCombatPos();
					Pos selfPos = self->GetCombatPos();
					if (targetPos.x > selfPos.x && targetPos.y > selfPos.y)
					{

						targetPos.x -= 88;
						targetPos.y -= 73;
					}
					else
					{
						targetPos.x += 88;
						targetPos.y += 73;

					}
					//self->SetCombatTargetPos({ enemy->GetCombatPos().x + 88 , enemy->GetCombatPos().y + 73 });
					self->SetCombatTargetPos(targetPos);
					self->SetCombatPos(self->GetPos());
					enemy->SetCombatPos(enemy->GetPos());


					self->SetTargetID((int)enemy->GetID());
					self->GetFSM()->ChangeState(PlayerCombatMoveState::GetInstance());

				}
				else if (command == u8"cast")
				{
					auto& self = m_Players[0];
					auto& enemy = m_Players[1];
					if (m_Player != self)
					{
						enemy = self;
						self = m_Player;
					}
					self->ClearTargets();
					self->AddTarget(enemy);
					self->GetFSM()->ChangeState(PlayerCombatCastAttackState::GetInstance());
				}
			}
			if (i != s_CommandSet.size() - 1) ImGui::SameLine();
		}

	}

	MESSAGE_DISPATCHER_INSTANCE->DispatchDelayedMessage();

	float dt = WINDOW_INSTANCE->GetDeltaTime();
	for (auto* player : m_Players)
	{
		if (!player->IsCombat())
		{
			player->SetIsCombat(true);
			player->SetHP(30.f);
			GAME_ENTITY_MANAGER_INSTANCE->RegisterEntity(player);
		}
		player->OnUpdate(dt);
	}
}



int scene_send_login_message(lua_State* L)
{
	int __argi__ = 1;
	int __argn__ = lua_gettop(L);
	const char* account = lua_tostring(L, __argi__++);
	int scene_id = (int)lua_tointeger(L, __argi__++);
	float pos_x = (float)lua_tonumber(L, __argi__++);
	float pos_y = (float)lua_tonumber(L, __argi__++);
	int role_id = (int)lua_tointeger(L, __argi__++);
	int weapon_id = (int)luaL_optinteger(L, __argi__, -1);
	if (weapon_id != -1)__argi__++;
	else weapon_id = 0;

	int dir = (int)luaL_optinteger(L, __argi__, -1);
	if (dir != -1)__argi__++;
	else dir = 0;


	return 0;
}

void luaopen_scene(lua_State* L)
{

}
