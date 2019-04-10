#include "scene.h"

#include "../animation/frame_animation.h"
#include "logger.h"
#include "cxrandom.h"
#include "../core/Renderer.h"
#include "../global.h"
#include "../combat/Combat.h"
#include "script_system.h"
#include "profile.h"
#include "scene_manager.h"
#include "input_manager.h"
#include "cxmath.h"
#include "actor/action.h"
#include "combat/Skill.h"
#include "resource_manager.h"
#include "actor/player_state.h"
#include <algorithm>
#include "net.h"
#include "script_system.h"
#include "utils.h"

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
		m_SmapTv = new ImageView(SmapWDF, m_SmapWas);
	}
	m_TransportUUIDs = utils::split_by_cuts(templ.at("transport_stations"), ',');
	
}

void Scene::OnLoad()
{
	//m_LocalPlayer = nullptr;
	m_Switching = true;
	m_NpcDialogBG = new FrameAnimation(WzifeWDF, 0x1732c1ef);
	s_Announcement = new TextView();
	s_Announcement->X = 479;
	s_Announcement->Y = 461;
	s_Announcement->PaddingHorizontal = 10;
	s_Announcement->PaddingVertical = 22;
	s_Announcement->BackgroundResID = RESOURCE_MANAGER_INSTANCE->EncodeWAS(WzifeWDF, 0x1732c1ef);

	s_Chat = new TextView();
	s_Chat->X = 0;
	s_Chat->Y = 577;
	s_Chat->PaddingHorizontal = 3;
	s_Chat->PaddingVertical = 3;
	s_Chat->BackgroundResID = RESOURCE_MANAGER_INSTANCE->EncodeWAS(WzifeWDF, 0x39D3BD99);
	s_Chat->OnEnterHit = [this]() {
		Player* player = GetLocalPlayer();
		player->Say(s_Chat->Text);
		s_Chat->TextCache.clear();
		net_send_chat_message(player->GetNickName(), utils::WstringToString(s_Chat->Text));
	};

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

	

	INPUT_MANAGER_INSTANCE->RegisterOnKeyClickEvent(GLFW_MOUSE_BUTTON_LEFT, [this]() {
		if (SCENE_MANAGER_INSTANCE->IsHoverImGui()) return;
		if (m_ShowSmap) return;
		if (m_LocalPlayer != nullptr)
		{
			auto mouseX = INPUT_MANAGER_INSTANCE->GetMouseX();
			auto mouseY = INPUT_MANAGER_INSTANCE->GetMouseY();
			Pos dest = GAME_INSTANCE->ScreenPosToMapPos({ mouseX, mouseY });
			m_LocalPlayer->MoveTo(m_Map,(int) dest.x, (int)dest.y);

			net_send_move_to_pos_message(m_LocalPlayer->GetNickName(), dest.x, dest.y);
		}
	});

	INPUT_MANAGER_INSTANCE->RegisterOnKeyClickEvent(GLFW_KEY_W, [this]() {
		if (m_LocalPlayer != nullptr)
		{
			m_LocalPlayer->TranslateY(-30);
		}
	});

	INPUT_MANAGER_INSTANCE->RegisterOnKeyClickEvent(GLFW_KEY_A, [this]() {
		if (m_LocalPlayer != nullptr)
		{
			m_LocalPlayer->TranslateX(-30);
		}
	});

	INPUT_MANAGER_INSTANCE->RegisterOnKeyClickEvent(GLFW_KEY_S, [this]() {
		if (m_LocalPlayer != nullptr)
		{
			m_LocalPlayer->TranslateY(30);
		}
	});

	INPUT_MANAGER_INSTANCE->RegisterOnKeyClickEvent(GLFW_KEY_D, [this]() {
		if (m_LocalPlayer != nullptr)
		{
			m_LocalPlayer->TranslateX(30);
		}
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

	for (auto* player: m_Players)
	{
		player->OnUpdate(dt);
	}

	if(s_Announcement)
		s_Announcement->OnUpdate();
	if(s_Chat)
		s_Chat->OnUpdate();

	if (m_ShowSmap && m_SmapTv)
	{
		m_SmapTv->OnUpdate();
	}

	
}

float amout = 1;
void Scene::ProcessInput()
{
	if(INPUT_MANAGER_INSTANCE->IsKeyUp(GLFW_KEY_W))
	{
		if (!m_LocalPlayer) return;
		m_LocalPlayer->TranslateY(-amout);
	};

	if(INPUT_MANAGER_INSTANCE->IsKeyUp(GLFW_KEY_A))
	{
		if (!m_LocalPlayer) return;
		m_LocalPlayer->TranslateX(-amout);
	};


	if(INPUT_MANAGER_INSTANCE->IsKeyUp(GLFW_KEY_S))
	{
		if (!m_LocalPlayer) return;
		m_LocalPlayer->TranslateY(amout);
	};


	if(INPUT_MANAGER_INSTANCE->IsKeyUp(GLFW_KEY_D))
	{
		if (!m_LocalPlayer) return;
		m_LocalPlayer->TranslateX(amout);
	};

	INPUT_MANAGER_INSTANCE->RegisterOnKeyClickEvent(GLFW_KEY_F,[this]()
	{
		if (!m_LocalPlayer) return;
		s_IsCombat=!s_IsCombat;
	});

	INPUT_MANAGER_INSTANCE->RegisterOnKeyClickEvent(GLFW_KEY_1,
		[this]() {
		if (!m_LocalPlayer) return;
		m_LocalPlayer->ChangeAction(0);
	}
	);

	INPUT_MANAGER_INSTANCE->RegisterOnKeyClickEvent(GLFW_KEY_2,
		[this]() {
		if (!m_LocalPlayer) return;
		m_LocalPlayer->ChangeRole(0);

	}
	);

	INPUT_MANAGER_INSTANCE->RegisterOnKeyClickEvent(GLFW_KEY_3,
		[this]() {
		if (!m_LocalPlayer) return;
		m_LocalPlayer->ChangeWeapon(0);
	}
	);

}

void Scene::Draw()
{
	if (m_Map== NULL)return;
	
	//先画一遍地图
	if (SCENE_MANAGER_INSTANCE->IsDrawMap()&& m_Map)
	{
		if(m_LocalPlayer == nullptr)
			m_Map->Draw(0, 0);
		else 
			m_Map->Draw(m_LocalPlayer->GetX(), m_LocalPlayer->GetY());
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

	for (auto* player: m_Players)
	{
		if (m_LocalPlayer == player)
		{
			int screenWidth = WINDOW_INSTANCE->GetWidth();
			int screenHeight = WINDOW_INSTANCE->GetHeight();
			int halfScreenWidth = screenWidth / 2;
			int halfScreenHeight = screenHeight / 2;
			int mapWidth = m_Map->GetWidth();
			int mapHeight = m_Map->GetHeight();

			int px = m_LocalPlayer->GetX();
			int py = m_LocalPlayer->GetY();

			int maxMapOffsetX = mapWidth - halfScreenWidth;
			int maxMapOffsetY = mapHeight - halfScreenHeight;

			px = px < halfScreenWidth ? px :
				(px > maxMapOffsetX ?
				(screenWidth - (mapWidth - px)) : halfScreenWidth);
			py = py < halfScreenHeight ? py :
				(py > maxMapOffsetY ?
				(screenHeight - (mapHeight - py)) : halfScreenHeight);
			if (SCENE_MANAGER_INSTANCE->IsDrawStrider())
				m_LocalPlayer->OnDraw(px, py);


			if (!m_Switching)
			{
				for (auto& it : s_TranportCircles)
				{
					if (utils::BoundHitTest(it.second->GetBound(), GAME_INSTANCE->MapPosToScreenPos(m_LocalPlayer->GetPos())))
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
					if (utils::BoundHitTest(it.second->GetBound(), GAME_INSTANCE->MapPosToScreenPos(m_LocalPlayer->GetPos())))
					{
						hitok = true;
						break;
					}
				}
				if(!hitok)
					m_Switching = false;
			}
			
		}
		
	}
	
	if (SCENE_MANAGER_INSTANCE->IsDrawMask() &&m_Map&& m_LocalPlayer)
		m_Map->DrawMask(m_LocalPlayer->GetX(), m_LocalPlayer->GetY(), m_LocalPlayer->GetDrawY());
    
	for (auto* player : m_Players)
	{
		if (m_LocalPlayer != player)
		{
			player->OnDraw(m_Map);
		}
	}

	if (GetLocalPlayer())GetLocalPlayer()->SetAutoRun(SCENE_MANAGER_INSTANCE->IsAutoRun());

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

BattleScene::BattleScene(int sceneID, String sceneName)
	:Scene(sceneID,sceneName)
{
	m_LocalPlayer = nullptr;
	Sprite* sp = RESOURCE_MANAGER_INSTANCE->LoadWASSprite(AddonWDF, 0x708C11A0, true);
	m_CombatBG = TextureManager::GetInstance()->LoadTexture(sp->mPath, sp->mWidth, sp->mHeight, true, (uint8_t*)sp->mFrames[0].src.data());
	RESOURCE_MANAGER_INSTANCE->UnLoadWASSprite(AddonWDF, 0x708C11A0);
}

BattleScene::~BattleScene()
{
	delete m_CombatBG;
	m_CombatBG = nullptr;
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
	if (ImGui::CollapsingHeader("CombatCommand"))
	{
		for (size_t i = 0; i < s_CommandSet.size(); i++)
		{
			std::string& command = s_CommandSet[i];
			if (ImGui::Button(command.c_str()) && m_Player != nullptr)
			{
				if (command == u8"attack")
				{
					
					auto& self =  m_Players[0];
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
		if(!player->IsCombat())
		{
			player->SetIsCombat(true);
			player->SetHP(30.f);
			GAME_ENTITY_MANAGER_INSTANCE->RegisterEntity(player);
		}
		player->OnUpdate(dt);
	}
}

void BattleScene::Draw()
{
	SPRITE_RENDERER_INSTANCE->DrawFrameSprite(m_CombatBG,
		glm::vec2(0, 0),
		glm::vec2(WINDOW_INSTANCE->GetWidth(), WINDOW_INSTANCE->GetHeight()));

	for (auto* player : m_Players)
	{
		auto pos = player->GetCombatPos();
		player->OnDraw((int)pos.x, (int)pos.y);
	}
}

void scene_send_login_message(const char* account , const char* pos_x_y)
{
	MSGC2SLogin msg;
	std::string pname(account);
	msg.type = PTO_C2S_LOGIN;
	msg.namelen = (int)pname.length();
	msg.name = pname;
	msg.scene_id = 1070;
	msg.dir = 1;
	msg.role_id = 9;
	msg.weapon_id = 5;
	auto poses = utils::split_by_cnt(pos_x_y, ',', 2);
	msg.pos_x = std::stof(poses[0]);
	msg.pos_y = std::stof(poses[1]);
	net_send_login_message(&msg);
}

void luaopen_scene(lua_State* L)
{
	script_system_register_function(L,scene_send_login_message);
	
}
