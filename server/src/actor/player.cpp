#include "player.h"
#include "cxmath.h"
#include "action.h"
#include "utils.h"

Player::Player(int roleID) :
	m_RoleID(roleID),
	m_NickName(""),
	m_WeaponID(-1),
	m_ActionID(Action::Idle),
	m_Pos(0, 0),
	m_MoveToPos(0, 0),
	m_Dir(Direction::S),
	m_IsMove(false),
	m_MoveVelocity(400),
	m_bInCombat(false),
	m_TargetID(-1),
	m_bSkillFrameShow(false)
{
	m_ActorType = ACTOR_TYPE_PLAYER;
	m_SayDuration = 0;
}

Player::~Player()
{

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

float Player::GetMoveDestDistSquare(Pos dest)
{
	return ::GMath::Astar_GetDistanceSquare(m_Pos.x, m_Pos.y, dest.x, dest.y);
}
float Player::GetMoveDestAngle(Pos dest)
{
	return ::GMath::Astar_GetAngle(m_Pos.x, m_Pos.y, dest.x, dest.y);
}


void Player::OnUpdate(float dt)
{

}

void Player::SetBox()
{
	m_Box.x = GetBoxX();
	m_Box.y = GetBoxY();
}

void Player::MoveTo(GameMap* gameMapPtr, int destX, int destY)
{
	m_BackupMoveList.clear();
	m_BackupMoveList = gameMapPtr->Move(GetBoxX(), GetBoxY(), destX / 20, destY / 20);
	if (m_BackupMoveList.size() < 2)return;
	m_BackupMoveList.pop_front();
	m_BackupMoveList.pop_back();

	m_MoveToPos.x = (float)destX;
	m_MoveToPos.y = (float)destY;

}

void Player::MoveToRandom()
{
	
}

void Player::Say(std::wstring Text)
{

}

bool Player::CanMove()
{
	return m_Pos.x != m_MoveToPos.x || m_Pos.y != m_MoveToPos.y;
}


Npc::Npc(const char* player_name, float x, float y, int dir, int role_id, int action_id, std::string msg)
	:Player(role_id)
{
	m_NickName = player_name;
	m_Pos.x = x;
	m_Pos.y = y;
	m_CombatPos.x = x;
	m_CombatPos.y = y;
	m_DirCount = 4;
	m_Dir = GMath::Dir8toDir4(dir);
	m_ActionID = action_id;
	m_ActorType = ACTOR_TYPE_NPC;
	m_WeaponID = -1;
	m_ShowDialog = false;
	m_DialogContent = msg;

}

Npc::~Npc()
{
}


Pet::Pet(const char* player_name, float x, float y, int dir, int role_id, int action_id)
	:Player(role_id)
{
	m_NickName = player_name;
	m_Pos.x = x;
	m_Pos.y = y;
	m_CombatPos.x = x;
	m_CombatPos.y = y;
	m_DirCount = 4;
	m_Dir = GMath::Dir8toDir4(dir);
	m_ActionID = action_id;
	m_ActorType = ACTOR_TYPE_PET;
	m_WeaponID = -1;

}

Pet::~Pet()
{

}

std::map<std::string, Player*> g_PlayerSet;

Player* actor_manager_create_player(int role_id,const char* name)
{
	std::string pname(name);
	auto it = g_PlayerSet.find(name);
	if (it == g_PlayerSet.end())
	{
		Player* player = new Player(role_id);
		player->SetNickName(name);
		g_PlayerSet.insert({ pname,player });
		return player;
	}
	return nullptr;
}

Player* actor_manager_find_player_by_name(const char* name)
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

void actor_manager_remove_player_by_name(const char* name)
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
	for (auto& it : g_PlayerSet)
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
