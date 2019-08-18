#include "player_state.h"
#include "player.h"
#include "combat/Skill.h"
#include "window.h"
#include "cxmath.h"
#include "resource_manager.h"
#include "cxrandom.h"
#include "actor/action.h"

void BasePlayerCombatState::Enter(Player* )
{

}
void BasePlayerCombatState::Execute(Player* player)
{

}
void BasePlayerCombatState::Exit(Player* )
{

}
bool BasePlayerCombatState::OnMessage(Player*, const Telegram&)
{
	return false;
}

void PlayerCombatIdleState::Enter(Player* player) 
{
	player->SetActionID(Action::Batidle);
}

void PlayerCombatIdleState::Execute(Player* player) 
{
	
};

void PlayerIdleState::Enter(Player* player) 
{
	if (!player->IsAutoRun())
	{
		player->SetDir(player->GetDir());
		player->SetActionID(Action::Idle);
	}
	else
	{
	//	player->MoveToRandom();
	}
	
}

void PlayerIdleState::Execute(Player* player) 
{

}

bool PlayerIdleState::OnMessage(Player* player, const Telegram& msg) 
{
	return false;
};

void PlayerMoveState::Enter(Player* player) 
{
	player->GetMoveList() = player->GetBackupMoveList();
	if (player->GetActionID() != Action::Walk)
		player->SetActionID(Action::Walk);
}

void PlayerMoveState::Execute(Player* player) 
{
	float dt = WINDOW_INSTANCE->GetDeltaTime();
	float localVelocity = player->GetVelocity()*dt;
	
	if (player->CanMove())
	{
		auto& moveList = player->GetMoveList();
		bool useMoveList = !moveList.empty();
		Pos dest;
		if (useMoveList)
		{
			Pos d = moveList.front();
			dest.x = d.x * 20 + 10;
			dest.y = d.y * 20 + 10;
		}
		else
		{
			dest.x = player->GetMoveToPos().x;
			dest.y = player->GetMoveToPos().y;
		}

		if (player->GetMoveDestDistSquare(dest) > localVelocity*localVelocity) {
			float degree = player->GetMoveDestAngle(dest);
			int m_Dir = player->GetDirByDegree(degree); 

			float stepRangeX = cos(DegreeToRadian(degree));
			float stepRangeY = sin(DegreeToRadian(degree));

			player->TranslateX(stepRangeX * localVelocity);
			player->TranslateY(stepRangeY * localVelocity);

			player->SetDir(m_Dir);
		}
		else
		{
			if (useMoveList)
			{
				moveList.pop_front();
			}	
			player->SetX(dest.x);
			player->SetY(dest.y);
			player->SetBox();
		}
	}
	else
	{
		player->SetBox();
		player->GetFSM()->ChangeState(PlayerIdleState::GetInstance());
	}
}

bool PlayerMoveState::OnMessage(Player* player, const Telegram& msg) 
{
	return false;
};

void PlayerCombatMoveState::Enter(Player* player) 
{
	m_bSent = false;
	player->SetActionID(Action::Runto);
	
	auto* playerFrame = player->GetCurrentPlayerFrame();
	if (!playerFrame)return;
	auto* weaponFrame = player->GetCurrentWeaponFrame();
	if (!weaponFrame)return;
    
	float dist_sqr = player->GetCombatDistSquare();
	float d = std::sqrt(dist_sqr) *1.0f / playerFrame->GetGroupFrameCount();
	//player->SetVelocity(1100);
	float localVelocity = player->GetVelocity();
	playerFrame->SetFrameTimeBase(d/localVelocity);
	weaponFrame->SetFrameTimeBase(d/localVelocity);
	weaponFrame->ResetFrameTimeByGroupCount(playerFrame->GetGroupFrameCount());
}

void PlayerCombatMoveState::Execute(Player* player) 
{	
	float dt = WINDOW_INSTANCE->GetDeltaTime();
	float localVelocity = player->GetVelocity()*dt;
	Pos combatPos = player->GetPos();
    
    float dist_sqr = player->GetCombatDistSquare();
	if (dist_sqr > localVelocity*localVelocity) {
		float degree = player->GetCombatAngle() ;
		float stepRangeX = cos(DegreeToRadian(degree));
		float stepRangeY = sin(DegreeToRadian(degree));
        player->SetPos(combatPos.x + stepRangeX*localVelocity , combatPos.y + stepRangeY*localVelocity);
	}
	else
	{
		//player->SetVelocity(400);
		player->GetFSM()->ChangeState(PlayerCombatAttackState::GetInstance());
	}
}

bool PlayerCombatMoveState::OnMessage(Player* player, const Telegram& msg) 
{
	//player->SetVelocity(400);
	player->GetFSM()->ChangeState(PlayerCombatAttackState::GetInstance());
	return true;
};

void PlayerCombatAttackState::Enter(Player* player) 
{
	player->SetActionID(Action::Attack);
	auto* playerFrame = player->GetCurrentPlayerFrame();
	if (!playerFrame)return;
	auto* weaponFrame = player->GetCurrentWeaponFrame();
	if (!weaponFrame)return;
	playerFrame->SetFrameTimeBase(1.0f/60*5);
	weaponFrame->SetFrameTimeBase(1.0f/60*5);
	weaponFrame->ResetFrameTimeByGroupCount(playerFrame->GetGroupFrameCount());
}

void PlayerCombatAttackState::Execute(Player* player) 
{
	auto* playerFrame = player->GetCurrentPlayerFrame();

	if (playerFrame->IsCurrentFrameChangedInUpdate() && (playerFrame->GetLastFrame() % playerFrame->GetGroupFrameCount()) == (int)(playerFrame->GetGroupFrameCount()*0.65))
	{
		MESSAGE_DISPATCHER_INSTANCE->DispatchMessageX(2, player->GetID(), player->GetTargetID(), (int)MSG_PlayerCombatBeAttackedState, nullptr);
	}
	if (playerFrame->IsNextFrameRestart())
	{
		//player->SetCombatPos(player->GetPos());			
		player->GetFSM()->ChangeState(PlayerCombatBackState::GetInstance());
	}
}
void PlayerCombatBackState::Enter(Player* player) 
{
	player->SetCombatTargetPos(player->GetPos());
	player->ReverseDir();

	player->SetActionID(Action::Runback);
	auto* playerFrame = player->GetCurrentPlayerFrame();
	if (!playerFrame)return;
	auto* weaponFrame = player->GetCurrentWeaponFrame();
	if (!weaponFrame)return;
	float dist_sqr = player->GetCombatDistSquare() ;
	float d = std::sqrt(dist_sqr) *1.0f / playerFrame->GetGroupFrameCount();
	//player->SetVelocity(950);
	float localVelocity = player->GetVelocity();
	playerFrame->SetFrameTimeBase(d/localVelocity);
	weaponFrame->SetFrameTimeBase(d/localVelocity);
	
	weaponFrame->ResetFrameTimeByGroupCount(playerFrame->GetGroupFrameCount());
	
}

void PlayerCombatBackState::Execute(Player* player) 
{
	float dt = WINDOW_INSTANCE->GetDeltaTime();
	float localVelocity = player->GetVelocity()*dt;
	if (player->GetCombatDistSquare() > localVelocity*localVelocity) {
		float degree = player->GetCombatAngle();
		float stepRangeX = cos(DegreeToRadian(degree));
		float stepRangeY = sin(DegreeToRadian(degree));
		Pos combatPos = player->GetPos();
		player->SetPos( combatPos.x +  stepRangeX*localVelocity,combatPos.y +  stepRangeY*localVelocity);
	}
	else
	{
		player->GetFSM()->ChangeState(PlayerCombatIdleState::GetInstance());
	}
}
void PlayerCombatBackState::Exit(Player* player) 
{
	player->ReverseDir();
}
	


void PlayerCombatBeAttackedState::Enter(Player* player) 
{
	player->AddHP(-10);
	player->SetActionID(Action::Behit);
    auto* playerFrame = player->GetCurrentPlayerFrame();
	if (!playerFrame)return;
    auto* weaponFrame = player->GetCurrentWeaponFrame();
	if (!weaponFrame)return;
    playerFrame->SetFrameTimeBase(1.0f / 60 * 4 );
    weaponFrame->SetFrameTimeBase(1.0f / 60 * 4 );
	
}
void PlayerCombatBeAttackedState::Execute(Player* player) 
{
	auto* playerFrame = player->GetCurrentPlayerFrame();
	if (playerFrame->IsNextFrameRestart())
	{
		if (player->GetHP() <= 0)
		{
			player->GetFSM()->ChangeState(PlayerCombatClpsState::GetInstance());
		}
		else
		{
			player->GetFSM()->ChangeState(PlayerCombatIdleState::GetInstance());
		}
		
	}
}


void PlayerCombatBeCastAttackedState::Enter(Player* player) 
{

}

void PlayerCombatBeCastAttackedState::Execute(Player* player) 
{
    auto& skillFrame = player->GetSkillFrame();
    if(skillFrame.GetCurrentFrame() == std::floor(skillFrame.GetGroupFrameCount()/3.0*2))
	{
		player->GetFSM()->ChangeState(PlayerCombatBeAttackedState::GetInstance());
	}
}


bool PlayerCombatGlobalState::OnMessage(Player* player, const Telegram& msg)
{
	if(msg.MsgType == MSG_PlayerCombatBeAttackedState)
	{
		player->GetFSM()->ChangeState(PlayerCombatBeAttackedState::GetInstance());
		return true;
	}
	else if(msg.MsgType == MSG_PlayerCombatBeCastAttackedState)
	{
		player->GetFSM()->ChangeState(PlayerCombatBeCastAttackedState::GetInstance());
		return true;
	}
	return false;
}
void PlayerCombatGlobalState::Enter(Player* player) 
{
}
void PlayerCombatGlobalState::Execute(Player* player) 
{
}
void PlayerCombatCastAttackState::Enter(Player* player) 
{
	player->SetActionID(Action::Cast);
	auto* playerFrame = player->GetCurrentPlayerFrame();
	if (!playerFrame)return;
	auto* weaponFrame = player->GetCurrentWeaponFrame();
	if (!weaponFrame)return;
	playerFrame->SetFrameTimeBase(1.0f/60*5);
	weaponFrame->SetFrameTimeBase(1.0f/60*5);
	weaponFrame->ResetFrameTimeByGroupCount(playerFrame->GetGroupFrameCount());
}

void PlayerCombatCastAttackState::Exit(Player* player)
{
	player->ClearTargets();
}

void PlayerCombatCastAttackState::Execute(Player* player) 
{
	auto* playerFrame = player->GetCurrentPlayerFrame();
		
	if(playerFrame->IsCurrentFrameChangedInUpdate() && playerFrame->GetLastFrame() % playerFrame->GetGroupFrameCount()== playerFrame->GetGroupFrameCount()/3)
	{
		SKILL_MANAGER_INSTANCE->SetRandomSkillID();

		auto id = SKILL_MANAGER_INSTANCE->GetRandomSkill();

		auto*skill = SKILL_MANAGER_INSTANCE->GetSkillByID(id);

		int delay = 0;
		for (auto* target : player->GetTargets())
		{
			target->SetSkillFrame(skill);
			MESSAGE_DISPATCHER_INSTANCE->DispatchMessageX(
				delay, player->GetID(), target->GetID(), MSG_PlayerCombatBeCastAttackedState, nullptr);
			delay += 2;
		}
	}
		
	if(playerFrame->IsNextFrameRestart())
	{
		player->GetFSM()->ChangeState(PlayerCombatIdleState::GetInstance());
	}
	
}

void PlayerCombatClpsState::Execute(Player* player)
{
	
}

void PlayerCombatClpsState::Enter(Player* player)
{
	player->SetActionID(Action::Clps);
}

void PlayerCombatClpsState::Exit(Player*)
{

}
