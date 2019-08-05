#include "player.h"
#include "logger.h"
#include "../message.h"
#include "../combat/Combat.h"
#include "../combat/Skill.h"
#include "cxrandom.h"
#include "player_state.h"
#include "profile.h"
#include "file_loading.h"
#include "scene/scene_manager.h"
#include "scene/Scene.h"
#include "cxmath.h"
#include "game.h"
#include "utils.h"
#include "window.h"


Player::Player(int roleID) :
	Actor(roleID),
	BaseGameEntity(),
	m_pFSM(nullptr),
	m_SkillFrame(nullptr)
{
	m_ActorType = ACTOR_TYPE_PLAYER;

	m_pFSM = new StateMachine<Player>(this);
	m_pFSM->SetCurrentState(PlayerIdleState::GetInstance());
	m_pFSM->SetGlobalState(PlayerCombatGlobalState::GetInstance());


	m_SayDuration = 0;

}


Player::~Player()
{
	if (m_pFSM)
	{
		delete m_pFSM;
		m_pFSM = nullptr;
	}

	ClearFrames();
	m_SkillFrame = nullptr;

}

void Player::ClearFrames()
{
	
}

float Player::GetCombatDistSquare()
{
	return ::GMath::Astar_GetDistanceSquare(m_CombatPos.x, m_CombatPos.y, m_CombatTargetPos.x, m_CombatTargetPos.y);
}

float Player::GetMoveDestDistSquare(Pos dest)
{
	return ::GMath::Astar_GetDistanceSquare(m_Pos.x, m_Pos.y, dest.x, dest.y);
}
float Player::GetCombatAngle()
{
	return ::GMath::Astar_GetAngle(m_CombatPos.x, m_CombatPos.y, m_CombatTargetPos.x, m_CombatTargetPos.y);
}
float Player::GetMoveDestAngle(Pos dest)
{
	return ::GMath::Astar_GetAngle(m_Pos.x, m_Pos.y, dest.x, dest.y);
}

void Player::ReloadFrames()
{
	ClearFrames();
}

void Player::ChangeRole(int roleID)
{
	m_RoleID = roleID;

	

}

void Player::ChangeWeapon(int WeaponID)
{
	m_WeaponID = WeaponID;
	
}

void Player::ChangeAction(int actionID)
{
	m_ActionID = actionID;
}

int Player::GetDirByDegree(float degree)
{
	if (m_DirCount == 4)
		return GMath::Astar_GetDir4(degree);
	else
		return GMath::Astar_GetDir(degree);
}

void Player::ReverseDir()
{
	switch (m_Dir)
	{
	case 6: m_Dir = 4; break;
	case 4: m_Dir = 6; break;
	case 5: m_Dir = 7; break;
	case 7: m_Dir = 5; break;
	case 3: m_Dir = 1; break;
	case 1: m_Dir = 3; break;
	case 2: m_Dir = 0; break;
	case 0: m_Dir = 2; break;
	}
	SetDir(m_Dir);
}

void Player::SaveFrame(int index)
{
	//TODO 
}


void Player::OnUpdate(float dt)
{
	if (m_pFSM)
	{
		m_pFSM->Update();
	}

	if (m_PlayerFrames.find(m_ActionID) != m_PlayerFrames.end())
	{
		m_PlayerFrames[m_ActionID]->OnUpdate();
	}
	else
	{
		auto playerFrameWasID = GAME_INSTANCE->GetActionWasID(m_ActorType, m_RoleID, m_ActionID);
		if (playerFrameWasID != -1) {
		
		}
	}

	if (m_WeaponFrames.find(m_ActionID) != m_WeaponFrames.end())
	{
		m_WeaponFrames[m_ActionID]->OnUpdate();
	}
	else
	{
		auto weaponFrameWasID = GAME_INSTANCE->GetWeaponWasID(m_WeaponID, m_ActionID);
		if (weaponFrameWasID != -1) {
			
		}
	}


	if (m_bInCombat)
	{
		if (m_SkillFrame != nullptr)
		{
			if (m_bSkillFrameShow)
			{
				m_SkillFrame->OnUpdate();
			}
			if (m_SkillFrame->IsNextFrameRestart())
			{
				m_bSkillFrameShow = false;
			}
		}
	}
}

int Player::GetDrawY()
{
	auto* frame = GetCurrentPlayerFrame();
	if (frame != nullptr && frame->GetSprite() != nullptr)
	{
		int current = frame->GetCurrentFrame();
		if (current >= frame->GetFrameCount())return GetY();
		auto& keyframeinfo = frame->GetSprite()->mFrames[current];
		int drawY = GetY() - frame->GetHeight() + frame->GetKeyY() - keyframeinfo.key_y;
		return drawY;
	}
	else
	{
		return GetY();
	}

}

FrameAnimation* Player::GetPlayerFrame(int actionID)
{
	auto it = m_PlayerFrames.find(actionID);
	if (it != m_PlayerFrames.end())
	{
		return it->second;
	}
	else
	{
		return nullptr;
	}
}

void Player::OnDraw(GameMap* gameMapPtr)
{
	if (!gameMapPtr) return;
	Pos pos = GAME_INSTANCE->MapPosToScreenPos(m_Pos);
	if (m_PlayerFrames.find(m_ActionID) != m_PlayerFrames.end())
	{
		auto& player = *m_PlayerFrames[m_ActionID];

		int _px = (int)pos.x - player.GetWidth() / 2 + 10;
		int _py = (int)pos.y - player.GetHeight() + 20;
		player.SetPos({ _px,_py });
		player.Draw();

		if (m_WeaponFrames.find(m_ActionID) != m_WeaponFrames.end())
		{
			auto& weapon = *m_WeaponFrames[m_ActionID];
			int px2 = _px - (weapon.GetKeyX() - player.GetKeyX());
			int py2 = _py - (weapon.GetKeyY() - player.GetKeyY());
			weapon.SetPos({ px2,py2 });
			weapon.Draw();
		}

		if (!m_NickName.empty())
		{
			auto green = glm::vec3(115 / 255.0f, 1.0f, 137 / 255.0f);
			
		}
	}

	if (m_SayDuration > 0)
	{
		 
	}

}

void Player::OnDraw(int px, int py)
{
	if (m_PlayerFrames.find(m_ActionID) != m_PlayerFrames.end())
	{
		auto& player = *m_PlayerFrames[m_ActionID];
		int _px = px - player.GetWidth() / 2 + 10;
		int _py = py - player.GetHeight() + 20;
		player.SetPos({ _px,_py });
		player.Draw();

		if (m_WeaponFrames.find(m_ActionID) != m_WeaponFrames.end())
		{
			auto& weapon = *m_WeaponFrames[m_ActionID];
			int px2 = _px - (weapon.GetKeyX() - player.GetKeyX());
			int py2 = _py - (weapon.GetKeyY() - player.GetKeyY());
			weapon.SetPos({ px2,py2 });
			weapon.Draw();
		}

		if (!m_NickName.empty())
		{
			auto green = glm::vec3(115 / 255.0f, 1.0f, 137 / 255.0f);
			
		}
		if (m_bSkillFrameShow && m_SkillFrame != nullptr)
		{
			int px2 = _px - (m_SkillFrame->GetKeyX() - player.GetKeyX());
			int py2 = _py - (m_SkillFrame->GetKeyY() - player.GetKeyY()) - 15;
			m_SkillFrame->SetPos({ px2,py2 });
			m_SkillFrame->Draw();
		}
	}

	if (m_SayDuration > 0)
	{
		int past = (int)WINDOW_INSTANCE->GetDeltaTimeMilliseconds();
		m_SayDuration -= past;

	
	}
}



void Player::SetBox()
{
	m_Box.x = GetBoxX();
	m_Box.y = GetBoxY();
}

void Player::MoveTo(GameMap* gameMapPtr, int destX, int destY)
{
	if (!gameMapPtr)return;
	m_BackupMoveList.clear();
	m_BackupMoveList = gameMapPtr->Move(GetBoxX(), GetBoxY(), destX / 20, destY / 20);
	if (m_BackupMoveList.size() < 2)return;
	m_BackupMoveList.pop_front();
	m_BackupMoveList.pop_back();

	m_MoveToPos.x = (float)destX;
	m_MoveToPos.y = (float)destY;

	m_pFSM->ChangeState(PlayerMoveState::GetInstance());
}

void Player::MoveTo(float x, float y)
{
	Scene* scene = dynamic_cast<Scene*>(SCENE_MANAGER_INSTANCE->GetCurrentScene());
	GameMap* gameMapPtr = scene->GetGameMap();
	if (!gameMapPtr)return;


	m_BackupMoveList.clear();
	m_BackupMoveList = gameMapPtr->Move(GetBoxX(), GetBoxY(), (int)(x / 20), (int)(y / 20));
	if (m_BackupMoveList.size() < 2)return;
	m_BackupMoveList.pop_front();
	m_BackupMoveList.pop_back();

	m_MoveToPos.x = x;
	m_MoveToPos.y = y;
	m_pFSM->ChangeState(PlayerMoveState::GetInstance());

}

void Player::MoveToRandom()
{
	Scene* scene = dynamic_cast<Scene*>(SCENE_MANAGER_INSTANCE->GetCurrentScene());
	GameMap* gameMapPtr = scene->GetGameMap();
	if (!gameMapPtr)return;

	IntPos dest = gameMapPtr->GetRandomPos();
	int destBoxX = dest.x;
	int destBoxY = dest.y;

	m_BackupMoveList.clear();
	m_BackupMoveList = gameMapPtr->Move(GetBoxX(), GetBoxY(), destBoxX, destBoxY);
	if (m_BackupMoveList.size() < 2)return;
	m_BackupMoveList.pop_front();
	m_BackupMoveList.pop_back();

	m_MoveToPos.x = destBoxX * 20.f + 10;
	m_MoveToPos.y = destBoxY * 20.f + 10;
	m_pFSM->ChangeState(PlayerMoveState::GetInstance());
}

void Player::ResetDirAll(int dir)
{
	for (auto& playerIt : m_PlayerFrames)
		playerIt.second->ResetAnim(dir);

	for (auto& weaponIt : m_WeaponFrames)
		weaponIt.second->ResetAnim(dir);
	m_Dir = dir;
}

void Player::ResetDir(int dir)
{
	for (auto& playerIt : m_PlayerFrames)
		playerIt.second->Reset(dir);

	for (auto& weaponIt : m_WeaponFrames)
		weaponIt.second->Reset(dir);
	m_Dir = dir;
}

void Player::SetDir(int dir)
{
	for (auto& playerIt : m_PlayerFrames)
		playerIt.second->SetCurrentGroup(dir);

	for (auto& weaponIt : m_WeaponFrames)
		weaponIt.second->SetCurrentGroup(dir);

	m_Dir = dir;
}

bool Player::HandleMessage(const Telegram& msg)
{
	return GetFSM()->HandleMessage(msg);
};

void Player::SetActionID(int state)
{
	m_ActionID = state;
	if (m_bInCombat)
	{
		if (m_PlayerFrames.find(m_ActionID) != m_PlayerFrames.end())
		{
			m_PlayerFrames[m_ActionID]->OnUpdate();
		}
		else
		{
			auto playerFrameWasID = GAME_INSTANCE->GetActionWasID(m_ActorType, m_RoleID, m_ActionID);
			if (playerFrameWasID != -1) {
			
			}
		}

		if (m_WeaponFrames.find(m_ActionID) != m_WeaponFrames.end())
		{
			m_WeaponFrames[m_ActionID]->OnUpdate();
		}
		else
		{
			auto weaponFrameWasID = GAME_INSTANCE->GetWeaponWasID(m_WeaponID, m_ActionID);
			if (weaponFrameWasID != -1) {

			}
		}
	}
}
FrameAnimation* Player::GetCurrentPlayerFrame()
{
	return m_PlayerFrames.find(m_ActionID) != m_PlayerFrames.end() ? m_PlayerFrames[m_ActionID] : nullptr;
};

FrameAnimation* Player::GetCurrentWeaponFrame()
{
	return m_WeaponFrames.find(m_ActionID) != m_WeaponFrames.end() ? m_WeaponFrames[m_ActionID] : nullptr;
};

void Player::SetSkillFrame(FrameAnimation* anim)
{
	if (!anim) return;
	m_bSkillFrameShow = true;
	m_SkillFrame = anim;
	m_SkillFrame->ResetAnim(0);
}

void Player::LogInfo()
{

}

void Player::Say(std::wstring Text)
{
	//m_SayDuration = 9000;
	m_SayDuration = 1000 * 60 * 24;
}

void Player::Say(std::string Text)
{
	std::wstring wText = utils::StringToWstring(Text);
	m_SayDuration = 1000 * 60 * 24;
}

bool Player::CanMove()
{
	return m_Pos.x != m_MoveToPos.x || m_Pos.y != m_MoveToPos.y;
}

float Player::GetWidth()
{
	auto* player = GetCurrentPlayerFrame();
	if (player == nullptr) return 0;
	return (float)player->GetWidth();
}

float Player::GetHeight()
{
	auto* player = GetCurrentPlayerFrame();
	if (player == nullptr) return 0;
	return (float)player->GetHeight();
}

Bound Player::GetScreenBound()
{
	auto* player = GetCurrentPlayerFrame();
	if (player == nullptr) return { 0,0,0,0 };
	auto screenIntPos = player->GetPos();
	Pos screenPos{ (float)screenIntPos.x,(float)screenIntPos.y };
	return Bound{ screenPos.x, (screenPos.x + GetWidth()),
		screenPos.y,(screenPos.y + GetHeight()) };
}

void Player::LoadRoleFrame()
{
	if (m_RoleID > 0)
	{
		
	}
}

void Player::LoadWeaponFrame()
{
	if (m_WeaponID > 0)
	{
		auto weaponFrameWasID = GAME_INSTANCE->GetWeaponWasID(m_WeaponID, m_ActionID);
		if (weaponFrameWasID != -1) {
		}
	}
}

void Player::SyncRoleWeaponFrame()
{
	if (m_PlayerFrames.find(m_ActionID) != m_PlayerFrames.end())
	{
		m_PlayerFrames[m_ActionID]->ResetAnim(m_Dir);
		auto groupCount = m_PlayerFrames[m_ActionID]->GetGroupFrameCount();
		m_WeaponFrames[m_ActionID]->ResetFrameTimeByGroupCount(groupCount);
	}
}

Npc::Npc(const char* player_name, float x, float y, int dir, int role_id, int action_id, std::string msg)
	:Player(role_id)
{
	m_ActorType = ACTOR_TYPE_NPC;
	m_NickName = player_name;
	m_Pos.x = x;
	m_Pos.y = y;
	m_CombatPos.x = x;
	m_CombatPos.y = y;
	m_DirCount = 4;
	m_Dir = GMath::Dir8toDir4(dir);
	m_ActionID = action_id;
	m_WeaponID = -1;
	m_ShowDialog = false;
	m_DialogContent = msg;

}

Npc::~Npc()
{
}

bool Npc::HandleMessage(const Telegram& msg) { return false; }



void Npc::ShowDialog(std::string msg)
{
	m_ShowDialog = true;
	m_DialogContent = msg;
}

void Npc::OnDraw(GameMap* m_GameMapPtr)
{
	

}

Pet::Pet(const char* player_name, float x, float y, int dir, int role_id, int action_id)
	:Player(role_id)
{
	m_ActorType = ACTOR_TYPE_PET;
	m_NickName = player_name;
	m_Pos.x = x;
	m_Pos.y = y;
	m_CombatPos.x = x;
	m_CombatPos.y = y;
	m_DirCount = 4;
	m_Dir = GMath::Dir8toDir4(dir);
	m_ActionID = action_id;
	m_WeaponID = -1;

}

void Pet::OnDraw(int px, int py)
{
	Player::OnDraw(px, py);
}

Pet::~Pet()
{

}

std::map<std::string, Player *> g_PlayerSet;

Player *actor_manager_create_player(int role_id, const char *name)
{
	std::string pname(name);
	auto it = g_PlayerSet.find(name);
	if (it == g_PlayerSet.end())
	{
		Player *player = new Player(role_id);
		player->SetNickName(name);
		g_PlayerSet.insert({pname, player});
		return player;
	}
	return nullptr;
}

Player *actor_manager_find_player_by_name(const char *name)
{
	auto it = g_PlayerSet.find(name);
	if (it != g_PlayerSet.end())
	{
		return it->second;
	}
	else
	{
		return nullptr;
	}
}

void actor_manager_remove_player_by_name(const char *name)
{
	auto it = g_PlayerSet.find(name);
	if (it != g_PlayerSet.end())
	{
		SafeDelete(it->second);
		g_PlayerSet.erase(it->first);
	}
}

void actor_manager_clear_player()
{
	for (auto &it : g_PlayerSet)
	{
		SafeDelete(it.second);
	}
	g_PlayerSet.clear();
}

std::vector<Player*> actor_manager_fetch_all_players()
{
	std::vector<Player*> players;
	for (auto& it : g_PlayerSet)
	{
		players.push_back(it.second);
	}
	return players;
}
