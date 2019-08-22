#include "player.h"
#include "text_renderer.h"
#include "resource_manager.h"
#include "input_manager.h"
#include "utils.h"
#include "cxmath.h"
#include "window.h"
#include "animation/frame_animation.h"
#include "scene/game_map.h"
#include "scene/base_scene.h"
#include "scene/scene_manager.h"
#include "actor/actor.h"
#include "actor/action.h"



Player::Player(uint64_t pid):
	Actor(pid)
{
	m_ActorType = ACTOR_TYPE_PLAYER;
	m_SayWidget = new TextView();
	m_SayWidget->PaddingHorizontal = 4;
	m_SayWidget->PaddingVertical = 2;
	m_SayWidget->ShowEmotion = true;
	m_SayWidget->BackgroundResID = RESOURCE_MANAGER_INSTANCE->EncodeWAS(WZIFEWDF, 0xEF073E43); //F3FAAAF2´ó¿ò
	//m_SayWidget->BackgroundResID = RESOURCE_MANAGER_INSTANCE->EncodeWAS(WZIFEWDF, 0xA4120EA9);
	m_SayDuration = 0;
}


Player::~Player()
{
	SafeDelete(m_SayWidget);
}


void Player::OnUpdate()
{
	float dt = WINDOW_INSTANCE->GetDeltaTimeMilliseconds();
	if (m_ASM)
	{
		
		m_ASM->Update();
	}

	if (m_SayDuration > 0)
	{
		if (m_SayWidget)
		{
			m_SayWidget->OnUpdate();
		}
	}
}

int Player::GetDrawY()
{
	return (int)m_Pos.y;
}


void Player::OnDraw(GameMap* gameMapPtr)
{
	//m_ASM->Draw();

	//if (!gameMapPtr) return;
	//Pos pos = GAME_INSTANCE->MapPosToScreenPos(m_Pos);
	
	if (!m_NickName.empty())
	{
		auto green = glm::vec3(115 / 255.0f, 1.0f, 137 / 255.0f);
		TextRenderer::GetInstance()->DrawTextC(m_NickName.c_str(),
			((int)m_Pos.x + 11),
			((int)m_Pos.y + 20),
			TextRenderer::CENTER
		);
	}
	

	if (m_SayDuration > 0)
	{
		int past = (int)WINDOW_INSTANCE->GetDeltaTimeMilliseconds();
		m_SayDuration -= past;

		if (m_SayWidget->Background != nullptr)
		{
			int bgWidth = m_SayWidget->Width;
			int bgHeight = m_SayWidget->Height;
			//auto& player = *m_PlayerFrames[m_ActionID];
			
			//m_SayWidget->X = (int)pos.x - bgWidth / 2 + 10;
			//sm_SayWidget->Y = (int)pos.y - player.GetHeight() + 20 - bgHeight;
		}
		m_SayWidget->OnDraw();
	}
	
}





void Player::ResetDirAll(int dir)
{
	m_Dir = dir;
}

void Player::ResetDir(int dir)
{
	m_Dir = dir;
}

void Player::SetDir(int dir)
{
	m_Dir = dir;
}


void Player::SetAction(int state)
{ 
	m_ActionID = state; 
}

void Player::Say(std::wstring Text)
{
	//m_SayDuration = 9000;
	m_SayDuration = 1000*60*24;
	m_SayWidget->TextCache.assign(Text.begin(),Text.end());
}

void Player::Say(std::string Text)
{
	std::wstring wText = utils::StringToWstring(Text);
	m_SayDuration = 1000 * 60 * 24;
	m_SayWidget->TextCache.assign(wText.begin(), wText.end());
}



Bound Player::GetScreenBound()
{
	return GetViewBounds();
	/*Pos screenPos{ (float)screenIntPos.x,(float)screenIntPos.y };
	return Bound{ screenPos.x, (screenPos.x + GetWidth()), 
		screenPos.y,(screenPos.y + GetHeight()) };*/
}


Npc::Npc(const char* player_name, float x, float y, int dir, int role_id, int action_id, std::string msg)
	:Player(role_id)
{
	m_ActorType = ACTOR_TYPE_NPC;
	m_NickName = player_name;
	m_Pos.x	 = x;
	m_Pos.y = y;
	m_DirCount = 4;
	m_Dir = GMath::Dir8toDir4(dir);
	m_ActionID = action_id;
	m_WeaponID = -1;
	m_ShowDialog = false;
	m_DialogContent = msg;

	INPUT_MANAGER_INSTANCE->RegisterView(this);
}

Npc::~Npc()
{
	INPUT_MANAGER_INSTANCE->UnRegisterView(this);
}

bool Npc::HandleMessage(const Telegram& msg) { return false; }


void Npc::OnClick(int button, int x, int y)
{
	m_ShowDialog = !m_ShowDialog;
} 

void Npc::OnGlobleClick(int x,int y)
{
	m_ShowDialog = false;
}


void Npc::ShowDialog(std::string msg)
{
	m_ShowDialog = true;
	m_DialogContent = msg;
}

void Npc::OnDraw(GameMap* m_GameMapPtr)
{  
	Scene* scene = dynamic_cast<Scene*>(SCENE_MANAGER_INSTANCE->GetCurrentScene());
	if (!scene)return;
	Player::OnDraw(m_GameMapPtr);

	auto* dialog = scene->GetNpcDialogBG();
	if (dialog && m_ShowDialog)
	{
		IntPos pos{ 0,0 };
		pos.x = (int)WINDOW_INSTANCE->GetWidth() / 2 - dialog->GetWidth() / 2;
		pos.y = (int)WINDOW_INSTANCE->GetHeight() -  dialog->GetHeight() -100;
		dialog->SetPos(pos);
		dialog->OnUpdate();
		dialog->Draw();
		if (m_DialogContent != "")
		{
			TextRenderer::GetInstance()->DrawTextC(
				m_DialogContent.c_str(),
				pos.x + 10,
				pos.y + 22,
				TextRenderer::LEFT,
				dialog->GetWidth() - 20,
				glm::vec3(0.f, 1.f, 1.f),
				true
			);
		}
		
	}
	
}

Pet::Pet(const char* player_name, float x, float y, int dir, int role_id, int action_id)
	:Player(role_id)
{
	m_ActorType = ACTOR_TYPE_PET;
	m_NickName = player_name;
	m_Pos.x = x;
	m_Pos.y = y;
	m_DirCount = 4;
	m_Dir = GMath::Dir8toDir4(dir);
	m_ActionID = action_id;
	m_WeaponID = -1;
	
}

void Pet::OnDraw(int px, int py)
{
}

Pet::~Pet()
{

}


