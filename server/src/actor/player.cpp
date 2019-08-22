#include "player.h"
#include "logger.h"
#include "message.h"
#include "combat/Combat.h"
#include "combat/Skill.h"
#include "cxrandom.h"
#include "profile.h"
#include "file_loading.h"
#include "scene/scene_manager.h"
#include "scene/scene.h"
#include "cxmath.h"
#include "utils.h"
#include "window.h"


Player::Player(uint64_t id) :
	Actor(id)
{
	m_ActorType = ACTOR_TYPE_PLAYER;


	m_SayDuration = 0;

}


Player::~Player()
{

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
}

int Player::GetDrawY()
{
	return 0;
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
}

void Player::ResetDirAll(int dir)
{
}

void Player::ResetDir(int dir)
{ 
	m_Dir = dir;
}

void Player::SetDir(int dir)
{
	

	m_Dir = dir;
}



void Player::SetActionID(int state)
{
	m_ActionID = state;
	
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
	return 0;
}

float Player::GetHeight()
{
	return 0;
}

Bound Player::GetScreenBound()
{
	return {};
}

