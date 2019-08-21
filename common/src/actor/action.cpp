#include "action.h"
#ifndef SIMPLE_SERVER
#include "utils.h"
#include "game.h"
#include "actor/move.h"
#include "actor/actor.h"
#include "resource_manager.h"
#include "animation/sprite.h"
#endif
#include "cxmath.h"

std::map<CXString, int> g_AttackKeyFrame ={
	{"FYN-DBSWORDS" ,5 },
	{"FYN-RING" ,9 },
	{"GJL-CLAW" ,7 },
	{"GJL-WAND" ,8 },
	{"HMR-CLAW" ,9 },
	{"HMR-WHIP" ,7 },
	{"HTG-AXE" ,7 },
	{"HTG-HAMMER" ,8 },
	{"JMW-AXE" ,7 },
	{"JMW-KNIFE" ,7 },
	{"JXK-KNIFE" ,4 },
	{"JXK-SWORD" ,6 },
	{"LTZ-FAN" ,8 },
	{"LTZ-SPEAR" ,6 },
	{"STB-HAMMER" ,6 },
	{"STB-SPEAR" ,10 },
	{"WTJ-RIBBON" ,8 },
	{"WTJ-RING" ,4 },
	{"XCE-RIBBON" ,7 },
	{"XCE-WAND" ,4 },
	{"XYS-FAN" ,4 },
	{"XYS-SWORD" ,5 },
	{"YNX-DBSWORDS" ,5 },
	{"YNX-WHIP" ,7 }
};

static std::vector<std::string> s_ActionSet = { u8"idle",u8"walk",u8"sit",u8"angry",u8"sayhi",u8"dance",u8"salute",u8"clps",u8"cry",u8"batidle",u8"attack",u8"cast",u8"behit",u8"runto",u8"runback",u8"defend",u8"unknown" };;
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

ActionStateMachine::~ActionStateMachine()
{
	for (auto& it : m_AvatarActions) {
		SafeDelete(it.second);
	}
	m_AvatarActions.clear();

	for (auto& it : m_WeaponActions) {
		SafeDelete(it.second);
	}
	m_WeaponActions.clear();
}
	
void ActionStateMachine::Update()
{
	if (m_AvatarID != m_Actor->GetAvatarID()) {
		SetAvatar(m_Actor->GetAvatarID());
	}
	if (m_HasWeapon != (m_Actor->GetWeaponAvatarID() != "")) {
		m_HasWeapon = (m_Actor->GetWeaponAvatarID() != "");
	}
	if (m_HasWeapon) {
		if (m_WeaponID != m_Actor->GetWeaponAvatarID()) {
			SetWeapon(m_Actor->GetWeaponAvatarID());
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
	avatar->Pos.x = pos.x - (float)avatar->KeyX;
	avatar->Pos.y = pos.y - (float)avatar->KeyY;

	avatar->Dir = dir;
	avatar->Draw();


	if (!HasWeapon() || !action_is_show_weapon(m_ActionID)) {
		if(m_pCurrentAction){
			m_pCurrentAction->Draw();
		}
		return;
	}
	auto* weapon = GetWeapon(m_ActionID);
	weapon->Pos.x = avatar->Pos.x + (float)(avatar->KeyX - weapon->KeyX);
	weapon->Pos.y = avatar->Pos.y + (float)(avatar->KeyY - weapon->KeyY);
	weapon->Dir = dir;
	weapon->Draw();

	if (m_pCurrentAction) {
		m_pCurrentAction->Draw();
	}	
}

void ActionStateMachine::SetWeapon(CXString id)
{
	for (auto& it : m_WeaponActions) {
		SafeDelete(it.second);
	}
	m_WeaponActions.clear();
	m_WeaponID = id;
	m_HasWeapon = id == "";
}

void ActionStateMachine::SetAvatar(CXString id)
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
		auto resid = RESOURCE_MANAGER_INSTANCE->GetActorActionResID(m_Actor->GetType(), m_AvatarID, action);
		m_AvatarActions[action] = new BaseSprite(resid);
		m_AvatarActions[action]->FrameInterval = m_TimeInterval;
		m_AvatarActions[action]->Dir = m_Actor->GetDir();
		m_AvatarActions[action]->Pos.x = m_Actor->GetPos().x - m_AvatarActions[action]->KeyX;
		m_AvatarActions[action]->Pos.y = m_Actor->GetPos().y - m_AvatarActions[action]->KeyY;
	}

	if (m_HasWeapon) {
		if (m_WeaponActions[action] == nullptr) {
			auto resid = RESOURCE_MANAGER_INSTANCE->GetActionResID(AVATAR_TYPE_WEAPON, m_WeaponID, action);
			m_WeaponActions[action] = new BaseSprite(resid);
			m_WeaponActions[action]->FrameInterval = m_TimeInterval;
			m_WeaponActions[action]->Dir = m_Actor->GetDir();
			m_WeaponActions[action]->Pos.x = m_Actor->GetPos().x - m_WeaponActions[action]->KeyX;
			m_WeaponActions[action]->Pos.y = m_Actor->GetPos().y - m_WeaponActions[action]->KeyY;

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
	SetAvatar(m_Actor->GetAvatarID());
	SetWeapon(m_Actor->GetWeaponAvatarID());
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
	pBeHitAnim = new Animation(ADDONWDF, 0x1D3FF13C);
	pBeHitAnim->Visible = false;
}

AttackAction::~AttackAction()
{
	delete pBeHitAnim;
}

void AttackAction::Update()
{
	auto* avatar = pASM->GetAvatar();
	if (!avatar)return;
	avatar->Update();

	if(pBeHitAnim->Visible){
		pBeHitAnim->Update();
	}
	int action = pASM->GetActionID();
	if (action == ACTION_BATIDLE) {
		if (avatar->bGroupEndUpdate) {
			pASM->SetAction(ACTION_RUNTO);
			if (m_Target) {
				Pos runto = m_Target->GetPos();
				auto* attackAvatar = pASM->GetAvatar(ACTION_ATTACK);
				auto* targetAvatar = m_Target->GetASM()->GetAvatar(ACTION_BEHIT);
				if(attackAvatar && targetAvatar){
					int dir = actor->GetDir();
					int attackKeyframe = g_AttackKeyFrame[actor->GetAvatarID()];
					if (attackKeyframe == 0)attackKeyframe = attackAvatar->AttackKeyFrame;
					auto* attackFrame = attackAvatar->GetFrame(dir*attackAvatar->GroupFrameCount + attackKeyframe);
					assert(targetAvatar->GroupFrameCount == 2);
					
					auto* targetFrame = targetAvatar->GetFrame(m_Target->GetDir()* 2 + 1);
					float x = runto.x ;
					float y = runto.y;
					if (dir == DIR_NE) {
						x = x - attackFrame->width -5;
						y = y - targetFrame->key_y + targetFrame->height *0.5f - attackFrame->height /2  +11;
					}
					else if (dir == DIR_NW) {
						x = x + 5;
						y = y - targetFrame->key_y + targetFrame->height *0.5f - attackFrame->height / 2 + 11;
					}
					else if (dir == DIR_SE) {
						x = x - attackFrame->width - 5;
						y = y - targetFrame->key_y + targetFrame->height *0.5f - attackFrame->height /2 -11;
					}
					else if (dir == DIR_SW) {
						x = x + 5;
						y = y - targetFrame->key_y + targetFrame->height *0.5f - attackFrame->height / 2 - 11;
					}
					runto.x = x + attackFrame->key_x;
					runto.y = y + attackFrame->key_y;
				}

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

			m_Target->SetActionID(ACTION_BATIDLE);
			
		}
		else if (avatar->bFrameUpdated) {
			int attackKeyframe = g_AttackKeyFrame[actor->GetAvatarID()];
			if (attackKeyframe == 0)attackKeyframe = avatar->AttackKeyFrame;
			if(avatar->CurrentFrame == attackKeyframe){
				auto* targetAvatar = m_Target->GetASM()->GetAvatar();
				if(targetAvatar){
					pBeHitAnim->Pos.x = targetAvatar->Pos.x + targetAvatar->KeyX - pBeHitAnim->KeyX ;
					pBeHitAnim->Pos.y = targetAvatar->Pos.y + targetAvatar->KeyY - pBeHitAnim->KeyY;
					pBeHitAnim->Visible = true;
				}
				m_Target->SetActionID(ACTION_BEHIT);
			}
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

void AttackAction::Draw()
{
	if(pBeHitAnim->Visible){
		pBeHitAnim->Draw();
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

	if (m_Target) {
		Pos runto = m_Target->GetPos();
		float degree = actor->GetMoveDestAngle(runto);
		int dir = actor->GetDirByDegree(degree);
		actor->SetDir(dir);
		m_Target->SetDir(dir);
		m_Target->ReverseDir();

		m_Target->GetASM()->SetAction(ACTION_BATIDLE);
	}

	pASM->SetAction(ACTION_BATIDLE);
	m_Velocity = actor->GetVelocity();
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

void IdleAction::Enter()
{
	auto* avatar = pASM->GetAvatar();
	if (avatar) {
		avatar->bPlay = false;
	}
}

#endif

