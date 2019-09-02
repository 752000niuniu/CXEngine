#include "scene.h"

#include "animation/frame_animation.h"
#include "logger.h"
#include "cxrandom.h"
#include "graphics/Renderer.h"
#include "combat/Combat.h"
#include "script_system.h"
#include "profile.h"
#include "scene_manager.h"
#include "input_manager.h"
#include "cxmath.h"
#include "actor/action.h"
#include "combat/Skill.h"
#include "resource_manager.h"
#include <algorithm>
#include "net.h"
#include "script_system.h"
#include "utils.h"
#include "actor/actor_manager.h"
#include "lua_bind.h"
#include "text_renderer.h"
#include "scene/game_map.h"
#include "actor/actor.h"
#include "actor/move.h"

bool s_IsCombat = true;
TextView* s_Announcement;
TextView* s_Chat;
std::map<String,FrameAnimation*> s_TranportCircles;

Scene::Scene(int id,String name)
:BaseScene(id,name)
{
	m_SmapTv = nullptr;
	m_SmapWas = -1;
	m_Switching = false;
}

Scene::~Scene()
{   
	//SafeDelete(m_SmapPic);
	SafeDelete(m_SmapTv);
}

void Scene::ApplyTemplate(const std::map<String, String>& templ)
{
	m_SmapWas = utils::tsv_parse_wasstr(templ.at("smap"));
	if (m_SmapWas)
	{
		m_SmapTv = new ImageView(SMAPWDF, m_SmapWas);
	}
	m_TransportUUIDs = utils::split_by_cuts(templ.at("transport_stations"), ',');
}

void Scene::OnLoad()
{
	//m_LocalPlayer = nullptr;
	m_Switching = true;
	m_NpcDialogBG = new FrameAnimation(WZIFEWDF, 0x1732c1ef);
	s_Announcement = new TextView();
	s_Announcement->X = 479;
	s_Announcement->Y = 461;
	s_Announcement->PaddingHorizontal = 10;
	s_Announcement->PaddingVertical = 22;
	s_Announcement->BackgroundResID = RESOURCE_MANAGER_INSTANCE->EncodeWAS(WZIFEWDF, 0x1732c1ef);

	s_Chat = new TextView();
	s_Chat->X = 0;
	s_Chat->Y = 577;
	s_Chat->PaddingHorizontal = 3;
	s_Chat->PaddingVertical = 3;
	s_Chat->BackgroundResID = RESOURCE_MANAGER_INSTANCE->EncodeWAS(WZIFEWDF, 0x39D3BD99);
	s_Chat->OnEnterHit = [this]() {
		script_system_call_function(script_system_get_luastate(), "on_player_send_chat_message", utils::WstringToString(s_Chat->Text).c_str());
	};

	for (auto id : m_TransportUUIDs)
	{
		auto* info = SCENE_MANAGER_INSTANCE->GetTransportStationInfo(id);
		if (info != nullptr)
		{
			auto* frame = new FrameAnimation(MAPANIWDF, 0x7F4CBC8C);
			frame->SetPos({ info->cx, info->cy });
			s_TranportCircles.insert({ id,frame });
		}
	}
	
	m_ShowSmap = false;

	INPUT_MANAGER_INSTANCE->RegisterOnKeyClickEvent(GLFW_KEY_F, [this]()
	{
		
		s_IsCombat = !s_IsCombat;
	});

	INPUT_MANAGER_INSTANCE->RegisterOnKeyClickEvent(GLFW_KEY_TAB, [this]() {
		m_ShowSmap = !m_ShowSmap;
	});

}

void Scene::OnUnLoad()
{
	SafeDelete(s_Announcement);
	SafeDelete(s_Chat);
	SafeDelete(m_NpcDialogBG);
	for (auto& it : s_TranportCircles)
	{
		SafeDelete(it.second);
	}
	s_TranportCircles.clear();
}

FrameAnimation* Scene::GetNpcDialogBG()
{
	return m_NpcDialogBG;
}

void Scene::OnSmapClick(float x, float y, float w, float h)
{
	Actor* player= actor_manager_fetch_local_player();
	if (player)
	{
		x = std::min(w, std::max(0.f, x));
		y = std::min(h, std::max(0.f, y));
		float dx = x / w * m_Map->GetMapWidth();
		float dy = y / h * m_Map->GetMapHeight();
		player->GetMoveHandle()->MoveTo(dx, dy);
	}
}

void Scene::Update()
{
	float dt = WINDOW_INSTANCE->GetDeltaTime();

	for (auto& it : s_TranportCircles)
	{
		it.second->OnUpdate();
	}
	
	actor_manager_update();

	if(s_Announcement)
		s_Announcement->OnUpdate();
	if(s_Chat)
		s_Chat->OnUpdate();

	if (m_ShowSmap && m_SmapTv)
	{
		m_SmapTv->OnUpdate();
	}
}


void Scene::Draw()
{
	if (m_Map== NULL)return;
	
	Actor* localPlayer = actor_manager_fetch_local_player();
	//先画一遍地图
	if (SCENE_MANAGER_INSTANCE->IsDrawMap()&& m_Map)
	{
		if(localPlayer == nullptr)
			m_Map->Draw(0, 0);
		else 
			m_Map->Draw(localPlayer->GetX(), localPlayer->GetY());
	}

	if (SCENE_MANAGER_INSTANCE->IsDrawCell()&&m_Map)
		m_Map->DrawCell();

	for (auto& it : s_TranportCircles)
	{
		auto* info = SCENE_MANAGER_INSTANCE->GetTransportStationInfo(it.first);
		auto* frame = it.second;
		Pos pos = GAME_INSTANCE->MapPosToScreenPos(Pos((float)info->cx, (float)info->cy));
		int _px = (int)pos.x - frame->GetWidth() / 2;
		int _py = (int)pos.y - frame->GetHeight() / 2;
		frame->SetPos({ _px,_py });
		frame->Draw();
	}

	if (!m_Switching)
	{
		for (auto& it : s_TranportCircles)
		{
			if (utils::BoundHitTest(it.second->GetBound(), GAME_INSTANCE->MapPosToScreenPos(localPlayer->GetPos())))
			{
				m_Switching = true;
				auto* info = SCENE_MANAGER_INSTANCE->GetTransportStationInfo(it.first);
				SCENE_MANAGER_INSTANCE->SwitchSceneByTransportUUID(info->to_uuid);
				//SCENE_MANAGER_INSTANCE->SwitchScene(1070);
				return;
			}
		}
	}
	else
	{
		bool hitok = false;
		for (auto& it : s_TranportCircles)
		{
			if (utils::BoundHitTest(it.second->GetBound(), GAME_INSTANCE->MapPosToScreenPos(localPlayer->GetPos())))
			{
				hitok = true;
				break;
			}
		}
		if (!hitok)
			m_Switching = false;
	}

	actor_manager_draw();

	if (m_Map&&localPlayer) {
		m_Map->DrawMask(localPlayer->GetX(), localPlayer->GetY(), localPlayer->GetY());
	}

	if(s_Chat)
	  s_Chat->OnDraw();
	
	if (SCENE_MANAGER_INSTANCE->IsDrawAnnounce()&& s_Announcement)
		s_Announcement->OnDraw();

	if (m_ShowSmap && m_SmapTv)
	{
		m_SmapTv->OnDraw();
	}
}


void Scene::SetAnnouncement(const char* text)
{
	if (s_Announcement)
	{
		std::wstring  wstr = utils::StringToWstring(utils::Utf8ToGB2312(text));
		s_Announcement->TextCache = std::vector<uint32_t>(wstr.begin(), wstr.end());
	}
}

void Scene::SetChat(const char* text)
{
	if (s_Chat)
	{
		std::wstring  wstr = utils::StringToWstring(utils::Utf8ToGB2312(text));
		s_Chat->TextCache = std::vector<uint32_t>(wstr.begin(), wstr.end());
	}
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


void clear_chat_text_cache()
{
	s_Chat->TextCache.clear();
}
void luaopen_scene(lua_State* L)
{
	script_system_register_function(L, clear_chat_text_cache);
		
}
