#include "action.h"
#include "resource_manager.h"
#include "utils.h"
#include "game.h"


Action::Action(Actor* actor) :
	m_Actor(actor),
	m_ID (Action::Idle)
{
	m_ASM = m_Actor->GetASM();
	m_ASM->EnsureLoadAction(m_ID);
}

BaseSprite* Action::OnUpdate(BaseSprite* avatar)
{
	avatar->Update();
	return avatar;
}

void Action::Update()
{
	auto* avatar = m_ASM->GetAvatar(m_ID);
	if (!avatar)return;

	avatar = OnUpdate(avatar);
	if (!avatar)return;
	
	if (!m_ASM->HasWeapon() ||!action_is_show_weapon(m_ID))return;
	auto* weapon = m_ASM->GetWeapon(m_ID);
	weapon->Update();
}

void Action::Draw()
{
	auto* avatar = m_ASM->GetAvatar(m_ID);
	if (!avatar)return;
	
	Pos pos = m_Actor->GetPos();
	int dir = m_Actor->GetDir();
	avatar->Pos.x = pos.x - (float)avatar->Width / 2 + 10;
	avatar->Pos.y = pos.y - (float)avatar->Height + 20;
	avatar->Dir = dir;
	avatar->Draw();

	if (!m_ASM->HasWeapon() || !action_is_show_weapon(m_ID))return;
	auto* weapon = m_ASM->GetWeapon(m_ID);
	weapon->Pos.x = avatar->Pos.x + (float)(avatar->KeyX - weapon->KeyX);
	weapon->Pos.y = avatar->Pos.y + (float)(avatar->KeyY - weapon->KeyY);
	weapon->Dir = dir;
	weapon->Draw();
}

void Action::SetID(int id)
{
	m_ID = id;
	m_ASM->EnsureLoadAction(m_ID);
	
	auto* avatar = m_ASM->GetAvatar(m_ID);
	avatar->Reset();

	if (!m_ASM->HasWeapon() || !action_is_show_weapon(m_ID))return;
	auto* weapon = m_ASM->GetWeapon(m_ID);
	weapon->Reset();
}

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
	if(action == Action::Sit
		||action == Action::Angry
		|| action == Action::Sayhi
		|| action == Action::Dance
		|| action == Action::Salute
		|| action == Action::Cry)
	{
		return false;
	}
	return true;
}

ActionStateMachine::ActionStateMachine(Actor* actor)
	:m_Actor(actor) 
{
	m_TimeInterval = 0.016f*4;
	m_pCurrentAction = nullptr;
	m_pPreviousAction = nullptr;
	for (int action = Action::Idle; action < Action::COUNT; action++) {
		m_AvatarActions.insert({ action,nullptr });
		m_WeaponActions.insert({ action,nullptr });
	}
	m_WeaponID = m_Actor->GetWeaponID();
	m_AvatarID = m_Actor->GetRoleID();
}

void ActionStateMachine::Update()
{
	if (m_AvatarID!= m_Actor->GetRoleID()) {
		SetAvatar(m_Actor->GetRoleID());
	}
	if (m_HasWeapon !=  (m_Actor->GetWeaponID() != 0)){
		m_HasWeapon = (m_Actor->GetWeaponID() != 0);
	}
	if (m_HasWeapon) {
		if (m_WeaponID != m_Actor->GetWeaponID()) {
			SetWeapon(m_Actor->GetWeaponID());
		}
	}

	if(m_pCurrentAction){
		m_pCurrentAction->Update();
	}
}

void ActionStateMachine::Draw()
{
	if (m_pCurrentAction) {
		m_pCurrentAction->Draw();
	}
}

void ActionStateMachine::SetWeapon(int id)
{
	for (auto& it : m_WeaponActions) {
		SafeDelete(it.second);
	}
	m_WeaponActions.clear();
	m_WeaponID = id;
	m_HasWeapon = m_WeaponID != 0;
}

void ActionStateMachine::SetAvatar(int id)
{
	for (auto& it : m_AvatarActions) {
		SafeDelete(it.second);
	}
	m_AvatarActions.clear();
	m_AvatarID = id;
}

void ActionStateMachine::RestoreAction()
{
	if(m_pCurrentAction){
		delete m_pCurrentAction;
		m_pCurrentAction = nullptr;
	}
	m_pCurrentAction = m_pPreviousAction;
}



void ActionStateMachine::ChangeAction(Action* action)
{
	if(m_pPreviousAction){
		delete m_pPreviousAction;
		m_pPreviousAction = nullptr;
	}
	m_pPreviousAction = m_pCurrentAction;
	m_pCurrentAction = action;	
}

void ActionStateMachine::EnsureLoadAction(int action)
{
	if (action < Action::Idle || action >= Action::COUNT)return;

	if (m_AvatarActions[action] == nullptr) {
		auto wasid = GAME_INSTANCE->GetActionWasID(ACTOR_TYPE_PLAYER, m_AvatarID, action);
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
	EnsureLoadAction(action);
	return m_AvatarActions[action];
}

BaseSprite* ActionStateMachine::GetWeapon(int action)
{
	EnsureLoadAction(action);
	return m_WeaponActions[action];
}

void ActionStateMachine::Reset()
{
	SetAvatar(m_Actor->GetRoleID());
	SetWeapon(m_Actor->GetWeaponID());
}

AttackAction::AttackAction(Actor* actor)
	:Action(actor)
{
	m_ID = Action::Batidle;
}

BaseSprite* AttackAction::OnUpdate(BaseSprite* avatar)
{
	avatar->Update();
	if(avatar->bGroupEndUpdate){
		if (m_ID == Action::Batidle) {
			SetID(Action::Runto);
		}else if(m_ID == Action::Runto){
			SetID(Action::Attack);
		}else if(m_ID == Action::Attack){
			SetID(Action::Runback);
			m_Actor->ReverseDir();
		}
		else if (m_ID == Action::Runback) {
			m_Actor->ReverseDir();
			auto* new_action = new Action(m_Actor);
			new_action->SetID(Action::Batidle);
			m_ASM->ChangeAction(new_action);
			return nullptr;
		}
	}
	return avatar;
}

MoveAction::MoveAction(Actor* actor)
	:Action(actor)
{

}

BaseSprite* MoveAction::OnUpdate(BaseSprite* avatar)
{
	return nullptr;
}
