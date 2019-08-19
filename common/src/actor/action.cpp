#include "action.h"
#ifndef SIMPLE_SERVER
#include "utils.h"
#include "game.h"
#include "actor/move.h"
#include "actor/actor.h"
#include "resource_manager.h"
#include "animation/sprite.h"
#endif

static std::vector<std::string> s_ActionSet = { u8"idle",u8"walk",u8"sit",u8"angry",u8"sayhi",u8"dance",u8"salute",u8"clps",u8"cry",u8"batidle",u8"attack",u8"cast",u8"behit",u8"runto",u8"runback",u8"defend" };;
std::string action_get_name(int i)
{
	return s_ActionSet[i];
}

size_t action_get_size()
{
	return s_ActionSet.size();
}

std::string action_system_get_action(int i)
{
	return s_ActionSet[i];
}

bool action_is_show_weapon(int action)
{
	if (action == ACTION_SIT
		|| action == ACTION_ANGRY
		|| action == ACTION_SAYHI
		|| action == ACTION_DANCE
		|| action == ACTION_SALUTE
		|| action == ACTION_CRY)
	{
		return false;
	}
	return true;
}

bool action_is_emotion_action(int action)
{
	if (action == ACTION_SIT
		|| action == ACTION_ANGRY
		|| action == ACTION_SAYHI
		|| action == ACTION_DANCE
		|| action == ACTION_SALUTE
		|| action == ACTION_CRY)
	{
		return true;
	}
	return false;
}

bool action_is_battle_action(int action)
{
	if (action == ACTION_CLPS
		|| action == ACTION_BATIDLE
		|| action == ACTION_ATTACK
		|| action == ACTION_CAST
		|| action == ACTION_BEHIT
		|| action == ACTION_RUNTO
		|| action == ACTION_RUNBACK
		|| action == ACTION_DEFEND)
	{
		return true;
	}
	return false;
}



#ifndef SIMPLE_SERVER
Action::Action(Actor* _actor) :
	actor(_actor)
{
	pASM = actor->GetASM();
}
 
void Action::Update()
{
	auto* avatar = pASM->GetAvatar();
	if (!avatar)return;
	avatar->Update();

	int action = actor->GetActionID();
	if (action != pASM->GetActionID()) {
		pASM->SetAction(action);
	}
}

void Action::Enter()
{

}

ActionStateMachine::ActionStateMachine(Actor* _actor)
	:m_Actor(_actor)
{
	m_TimeInterval = 0.016f * 4;
	m_pCurrentAction = nullptr;
	m_pPreviousAction = nullptr;
	for (int action = ACTION_IDLE; action < ACTION_COUNT; action++) {
		m_AvatarActions.insert({ action,nullptr });
		m_WeaponActions.insert({ action,nullptr });
	}
	m_WeaponID = m_Actor->GetWeaponID();
	m_AvatarID = m_Actor->GetRoleID();
	m_ActionID = ACTION_IDLE;
}

void ActionStateMachine::Update()
{
	if (m_AvatarID != m_Actor->GetRoleID()) {
		SetAvatar(m_Actor->GetRoleID());
	}
	if (m_HasWeapon != (m_Actor->GetWeaponID() != 0)) {
		m_HasWeapon = (m_Actor->GetWeaponID() != 0);
	}
	if (m_HasWeapon) {
		if (m_WeaponID != m_Actor->GetWeaponID()) {
			SetWeapon(m_Actor->GetWeaponID());
		}
	}
	
	if (m_pCurrentAction) {
		m_pCurrentAction->Update();
	}

	auto* avatar = GetAvatar(m_ActionID);
	if (!avatar)return;
	
	if (!HasWeapon() || !action_is_show_weapon(m_ActionID))return;
	auto* weapon = GetWeapon(m_ActionID);
	weapon->CurrentFrame = avatar->CurrentFrame;
}

void ActionStateMachine::Draw()
{
	auto* avatar = GetAvatar(m_ActionID);
	if (!avatar)return;

	Pos pos = m_Actor->GetPos();
	int dir = m_Actor->GetDir();
	avatar->Pos.x = pos.x - (float)avatar->Width / 2 + 10;
	avatar->Pos.y = pos.y - (float)avatar->Height + 20;
	avatar->Dir = dir;
	avatar->Draw();

	if (!HasWeapon() || !action_is_show_weapon(m_ActionID))return;
	auto* weapon = GetWeapon(m_ActionID);
	weapon->Pos.x = avatar->Pos.x + (float)(avatar->KeyX - weapon->KeyX);
	weapon->Pos.y = avatar->Pos.y + (float)(avatar->KeyY - weapon->KeyY);
	weapon->Dir = dir;
	weapon->Draw();
}

void ActionStateMachine::SetWeapon(int id)
{
	for (auto& it : m_WeaponActions) {
		SafeDelete(it.second);
	}
	m_WeaponActions.clear();
	m_WeaponID = id;
	m_HasWeapon = m_WeaponID >= 0;
}

void ActionStateMachine::SetAvatar(int id)
{
	for (auto& it : m_AvatarActions) {
		SafeDelete(it.second);
	}
	m_AvatarActions.clear();
	m_AvatarID = id;
}

void ActionStateMachine::SetAction(int id)
{
	m_ActionID = id;
	EnsureLoadAction(m_ActionID);

	auto* avatar = GetAvatar(m_ActionID);
	avatar->Reset();
	if (!HasWeapon() || !action_is_show_weapon(m_ActionID))return;
	auto* weapon = GetWeapon(m_ActionID);
	weapon->Reset();
}

void ActionStateMachine::RestoreAction()
{
	if (m_pCurrentAction) {
		delete m_pCurrentAction;
		m_pCurrentAction = nullptr;
	}
	m_pCurrentAction = m_pPreviousAction;
}

void ActionStateMachine::ChangeAction(Action* action)
{
	if (m_pCurrentAction) {
		m_pCurrentAction->Exit();
	}
	if (m_pPreviousAction) {
		delete m_pPreviousAction;
		m_pPreviousAction = nullptr;
	}
	m_pPreviousAction = m_pCurrentAction;
	m_pCurrentAction = action;
	if (m_pCurrentAction) {
		m_pCurrentAction->Enter();
	}
}

void ActionStateMachine::EnsureLoadAction(int action)
{
	if (action < ACTION_IDLE || action >= ACTION_COUNT)return;

	if (m_AvatarActions[action] == nullptr) {
		auto wasid = GAME_INSTANCE->GetActionWasID(m_Actor->GetType(), m_AvatarID, action);
		m_AvatarActions[action] = new BaseSprite(ShapeWDF, wasid);
		m_AvatarActions[action]->FrameInterval = m_TimeInterval;
		m_AvatarActions[action]->Dir = m_Actor->GetDir();
	}

	if (m_HasWeapon) {
		if (m_WeaponActions[action] == nullptr) {
			auto wasid = GAME_INSTANCE->GetWeaponWasID(m_WeaponID, action);
			m_WeaponActions[action] = new BaseSprite(ShapeWDF, wasid);
			m_WeaponActions[action]->FrameInterval = m_TimeInterval;
			m_WeaponActions[action]->Dir = m_Actor->GetDir();
		}
	}
}

BaseSprite* ActionStateMachine::GetAvatar(int action)
{
	if (action == -1)action = m_ActionID;
	EnsureLoadAction(action);
	return m_AvatarActions[action];
}

BaseSprite* ActionStateMachine::GetWeapon(int action)
{
	if (action == -1)action = m_ActionID;
	EnsureLoadAction(action);
	return m_WeaponActions[action];
}

void ActionStateMachine::Reset()
{
	SetAvatar(m_Actor->GetRoleID());
	SetWeapon(m_Actor->GetWeaponID());
}

int ActionStateMachine::GetDirCount(int action)
{
	if (action == -1)action = m_ActionID;
	if (m_AvatarActions[action]) {
		return m_AvatarActions[action]->GroupCount;
	}
	return 4;
}

AttackAction::AttackAction(Actor* actor)
	:Action(actor)
{
	
}

AttackAction::~AttackAction()
{

}

void AttackAction::Update()
{
	auto* avatar = pASM->GetAvatar();
	if (!avatar)return;
	avatar->Update();

	int action = pASM->GetActionID();
	if (action == ACTION_BATIDLE) {
		if (avatar->bGroupEndUpdate) {
			pASM->SetAction(ACTION_RUNTO);

			if (m_Target) {
				Pos runto = m_Target->GetPos();
				actor->GetMoveHandle()->MoveTo(runto.x, runto.y);
				avatar = pASM->GetAvatar();

				float dist = std::sqrt(actor->GetMoveDestDistSquare(runto));
				float perframetime = 0.016f * 6;
				float perframe_dist = dist / avatar->GroupFrameCount;
				float velocity = perframe_dist / perframetime;
				avatar->FrameInterval = perframetime;
				actor->SetVelocity(velocity);
			}
		}
		else {

		}
	}
	else if (action == ACTION_RUNTO) {
		if (avatar->bGroupEndUpdate) {
			
			pASM->SetAction(ACTION_ATTACK);
			avatar = pASM->GetAvatar();
			avatar->FrameInterval = avatar->FrameInterval * 2;
		}
	}
	else if (action == ACTION_ATTACK) {
		if (avatar->bGroupEndUpdate) {
			pASM->SetAction(ACTION_RUNBACK);
			actor->ReverseDir();
			actor->GetMoveHandle()->MoveTo(m_BackupPos.x, m_BackupPos.y);

			avatar = pASM->GetAvatar();

			float dist = std::sqrt(actor->GetMoveDestDistSquare(m_BackupPos));
			float perframetime = 0.016f * 6;
			float perframe_dist = dist / avatar->GroupFrameCount;
			float velocity = perframe_dist / perframetime;
			avatar->FrameInterval = perframetime;
			actor->SetVelocity(velocity);
		}

	}
	else if (action == ACTION_RUNBACK) {
		if (avatar->bGroupEndUpdate) {
			actor->ReverseDir();
			actor->SetActionID(ACTION_BATIDLE);
			auto* new_action = new Action(actor);
			pASM->ChangeAction(new_action);
		}
	}
}

void AttackAction::AddTarget(Actor* target)
{
	m_Target = target;
}

void AttackAction::Exit()
{
	actor->SetPos(m_BackupPos);
	actor->SetVelocity(m_Velocity);
}

void AttackAction::Enter()
{
	m_BackupActionID = actor->GetActionID();
	m_BackupPos = actor->GetPos();
	pASM->SetAction(ACTION_BATIDLE);
	
	m_Velocity = actor->GetVelocity();

	if (m_Target) {
		m_Target->SetDir(actor->GetDir());
		m_Target->ReverseDir();
	}
}

BeHitAction::BeHitAction(Actor* actor, Actor* attacker)
	:Action(actor),
	m_Attacker(attacker)
{

}

void BeHitAction::Update()
{
	auto* avatar = pASM->GetAvatar();
	if (!avatar)return;
	avatar->Update();
}
void BeHitAction::Exit()
{

}
void BeHitAction::Enter()
{

}
#endif