#include "action.h"
#ifndef SIMPLE_SERVER
#include "utils.h"
#include "game.h"
#include "actor/move.h"
#include "actor/actor.h"
#include "resource_manager.h"
#include "animation/sprite.h"
#include "texture_manager.h"
#endif
#include "cxmath.h"
#include "time/timer_manager.h"
#include "window.h"
#include "scene/base_scene.h"
#include "scene/game_map.h"
#include "actor_manager.h"
#include "scene/scene_manager.h"
#include "script_system.h"
#include "logger.h"



#define  PERFRAME_TIME (0.016f*2.5f)
std::map<CXString, int> g_AttackKeyFrame = {
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

void call_combat_system_on_acting_end(Actor* actor)
{
	lua_State* L = script_system_get_luastate();
	lua_getglobal(L, "combat_system_on_acting_end");
	lua_push_actor(L, actor);
	int res = lua_pcall(L, 1, 0, 0);
	check_lua_error(L, res);
}



#ifndef SIMPLE_SERVER

Animation* CreateBuffAnimation(uint64_t resid) {
	if (resid == 0)return nullptr;
	Animation* anim = new Animation(resid);
	anim->SetLoop(0);
	return anim;
}

bool CalcRunToPos(Actor* actor, Actor* target, Pos& runto) {
	auto* attackAvatar = actor->GetASM()->GetAvatar(ACTION_ATTACK);
	auto* targetAvatar = target->GetASM()->GetAvatar(ACTION_BEHIT);
	if (attackAvatar && targetAvatar) {
		int attackKeyframe = g_AttackKeyFrame[actor->GetProperty(PROP_AVATAR_ID).toString()];
		if (attackKeyframe == 0)attackKeyframe = attackAvatar->GetAttackKeyFrame();
		int dir = actor->GetDir();
		auto* attackFrame = attackAvatar->GetFrame(dir*attackAvatar->GroupFrameCount + attackKeyframe);
		auto* targetFrame = targetAvatar->GetFrame(GMath::GetReverseDir(dir) * targetAvatar->GroupFrameCount + targetAvatar->GroupFrameCount - 1);
		float x = (float)target->GetX();
		float y = (float)target->GetY();
		if (dir == DIR_NE) {
			x = x - attackFrame->Width - 5;
			y = y - targetFrame->KeyY + targetFrame->Height *0.5f - attackFrame->Height / 2 + 11;
		}
		else if (dir == DIR_NW) {
			x = x + 5;
			y = y - targetFrame->KeyY + targetFrame->Height *0.5f - attackFrame->Height / 2 + 11;
		}
		else if (dir == DIR_SE) {
			x = x - attackFrame->Width - 5;
			y = y - targetFrame->KeyY + targetFrame->Height *0.5f - attackFrame->Height / 2 - 11;
		}
		else if (dir == DIR_SW) {
			x = x + 5;
			y = y - targetFrame->KeyY + targetFrame->Height *0.5f - attackFrame->Height / 2 - 11;
		}
		x = x + attackFrame->KeyX;
		y = y + attackFrame->KeyY;
		Pos src = actor->GetPos();
		runto.x = x - src.x;
		runto.y = y - src.y;
		return true;
	}
	return false;
}

Action::Action(Actor* _actor) :
	m_Actor(_actor),
	m_Type(ASM_ACTION)
{
	m_pASM = m_Actor->GetASM();
}

void Action::Update()
{
	int action = m_Actor->GetProperty(PROP_ACTION_ID).toInt();
	if (action != m_pASM->GetActionID()) {
		m_pASM->SetAction(action);
	}

	auto* avatar = m_pASM->GetAvatar();
	if (!avatar)return;
	avatar->Update();
} 


void Action::OnEnter()
{
#ifndef SIMPLE_SERVER
	lua_State* L = script_system_get_luastate();
	lua_getglobal(L, "asm_action_on_enter");
	lua_push_actor(L, m_Actor);
	lua_pushinteger(L, m_Type);
	int res = lua_pcall(L, 2, 0, 0);
	check_lua_error(L, res);
#endif	
	Enter();
}

void Action::OnExit()
{
#ifndef SIMPLE_SERVER
	lua_State* L = script_system_get_luastate();
	lua_getglobal(L, "asm_action_on_exit");
	lua_push_actor(L, m_Actor);
	lua_pushinteger(L, m_Type);
	int res = lua_pcall(L, 2, 0, 0);
	check_lua_error(L, res);
#endif
	Exit();
}

void Action::Enter()
{
	int action = m_Actor->GetProperty(PROP_ACTION_ID).toInt();
	if (action != m_pASM->GetActionID()) {
		m_pASM->SetAction(action);
	}

	auto* avatar = m_pASM->GetAvatar();
	if (!avatar)return;
	avatar->Reset();
}

void AttackAction::Update()
{
	int action = m_pASM->GetActionID();
	auto* avatar = m_pASM->GetAvatar();
	if (!avatar)return;
	avatar->Update();
	if (action == ACTION_RUNTO) {
		if (avatar->IsGroupEndUpdate()) {
			m_pASM->SetAction(ACTION_ATTACK);
			
			lua_State* L = script_system_get_luastate();
			lua_getglobal(L, "on_atk_skill_launch");
			lua_push_actor(L, m_Actor);
			int res = lua_pcall(L, 1, 0, 0);
			check_lua_error(L, res);
		}
	}
	else if (action == ACTION_ATTACK) {
		//if (avatar->IsGroupEndUpdate()) {
		//	script_system_call_function(script_system_get_luastate(), "on_atk_skill_");
		//	/*m_ComboCount = m_ComboCount - 1;
		//	if (m_ComboCount == 0 || m_Target->GetProperty(PROP_IS_DEAD).toBool()) {
		//		m_pASM->SetAction(ACTION_RUNBACK);
		//		m_Actor->ReverseDir();
		//		avatar = m_pASM->GetAvatar();
		//		if (!avatar)return;
		//		avatar->FrameInterval = PERFRAME_TIME;
		//		m_Actor->GetMoveHandle()->MoveOnScreenWithDuration({ -m_Runto.x,-m_Runto.y }, avatar->GetGroupFrameTime(), true);
		//	}
		//	else {
		//		m_pASM->SetAction(ACTION_ATTACK);
		//	}*/
		//}
		//else if (avatar->IsFrameUpdate()) {
		//	int attackKeyframe = g_AttackKeyFrame[m_Actor->GetProperty(PROP_AVATAR_ID).toString()];
		//	if (attackKeyframe == 0)attackKeyframe = avatar->GetAttackKeyFrame();
		//	if (avatar->CurrentFrame == attackKeyframe) {
		//		BeHitAction* behit = new BeHitAction(m_Target, m_Actor);
		//		behit->MoveVec = m_Actor->GetAttackVec();
		//		m_Target->GetASM()->ChangeAction(behit);
		//		if(m_Target->GetProperty(PROP_IS_DEAD).toBool()){
		//			avatar->Pause(500);
		//		}else{
		//			avatar->Pause(200);
		//		}
		//		if (m_ComboCount == 1) {
		//			uint64_t resid = m_Actor->GetProperty(PROP_ASM_BUFF_ANIM).toUInt64();
		//			auto* anim = CreateBuffAnimation(resid);
		//			m_pASM->AddStateAnim(anim);
		//		}
		//	}
		//}
	}
	else if (action == ACTION_RUNBACK) {
		if (avatar->IsGroupEndUpdate()) {
			m_Actor->ReverseDir();

		
			m_Actor->SetActionID(ACTION_BATIDLE);
			m_Actor->GetASM()->RestoreAction();
			//auto* new_action = new Action(m_Actor);
			//m_pASM->ChangeAction(new_action);*/
		}
	}
}

void AttackAction::Draw()
{
}

void AttackAction::Exit()
{
	lua_State* L = script_system_get_luastate();
	lua_getglobal(L, "on_atk_skill_end");
	lua_push_actor(L, m_Actor);
	int res = lua_pcall(L, 1, 0, 0);
	check_lua_error(L, res);

	m_Actor->SetPos(m_BackupPos);
	m_Actor->SetProperty(PROP_MOVE_VELOCITY, m_SavedVelocity);
	m_Actor->SetProperty(PROP_COMBAT_ACTING, false);
	call_combat_system_on_acting_end(m_Actor);
}


void AttackAction::Enter()
{
	m_ComboCount = m_Actor->GetProperty(PROP_ASM_ATK_COMBO).toInt();
	m_BackupActionID = m_Actor->GetProperty(PROP_ACTION_ID).toInt();
	m_BackupPos = m_Actor->GetPos();

	m_SavedVelocity = m_Actor->GetProperty(PROP_MOVE_VELOCITY).toFloat();

	//Sm_Target = m_Actor->GetTarget();
	//if (m_Target) {
	//	float degree = m_Actor->GetMoveDestAngle(m_Target->GetPos());
	//	int dir = m_Actor->GetDirByDegree(degree);
	//	dir = GMath::Dir8toDir4(dir);
	//	m_Actor->SetDir(dir);
	//	m_Target->SetDir(dir);
	//	m_Target->ReverseDir();
	//	 
	//	//m_Target->SetActionID(ACTION_BATIDLE);
	//	CalcRunToPos(m_Actor, m_Target, m_Runto);
	//}
	//m_pASM->SetAction(ACTION_RUNTO);
	
	lua_State* L = script_system_get_luastate();
	lua_getglobal(L, "on_atk_skill_start");
	lua_push_actor(L, m_Actor);
	int res = lua_pcall(L, 1, 0, 0);
	check_lua_error(L, res);

	m_Actor->SetProperty(PROP_COMBAT_ACTING, true);

	/*if (m_Target) {
		auto* avatar = m_pASM->GetAvatar();
		if (!avatar)return;
		avatar->FrameInterval = PERFRAME_TIME;
		m_Actor->GetMoveHandle()->MoveOnScreenWithDuration(m_Runto, avatar->GetGroupFrameTime(), true);
	}*/
}


void CastAction::Update()
{
	auto* avatar = m_pASM->GetAvatar();
	if (!avatar)return;
	avatar->Update();

	int action = m_pASM->GetActionID();
	if (action == ACTION_BATIDLE) {
		if (avatar->IsGroupEndUpdate()) {
			m_pASM->SetAction(ACTION_CAST);
		}
	}
	else if (action == ACTION_CAST) {
		if (avatar->IsGroupEndUpdate()) {
			m_Actor->SetActionID(ACTION_BATIDLE);
			auto* new_action = new Action(m_Actor);
			m_pASM->ChangeAction(new_action);
			m_pASM->AddDelayCallback(1000, [this]() {
				m_Actor->SetProperty(PROP_COMBAT_ACTING, false);
				call_combat_system_on_acting_end(m_Actor);
			});
		}
		else if (avatar->IsFrameUpdate()) {
			if (m_Actor->GetProperty(PROP_ASM_PLAY_BEHIT).toBool()) {
				if (avatar->CurrentFrame == avatar->GroupFrameCount / 2) {
					BeCastAction* action = new BeCastAction(m_Target, m_Actor);
					action->MoveVec = m_Actor->GetAttackVec();
					m_Target->GetASM()->ChangeAction(action);
				}
			}
			else {
				if (avatar->CurrentFrame == avatar->GroupFrameCount /2) {
					uint64_t id = m_Actor->GetProperty(PROP_CAST_ID).toUInt64();
					Animation* anim = new Animation(id);
					anim->SetLoop(1);
					auto* targetAvatar = m_Target->GetASM()->GetAvatar();
					anim->Pos.x = targetAvatar->Pos.x;
					anim->Pos.y = targetAvatar->Pos.y - targetAvatar->GetFrameKeyY() + targetAvatar->GetFrameHeight() / 2;
					anim->AddFrameCallback(anim->GroupFrameCount/6*5, [this, anim]() {
						uint64_t resid = m_Actor->GetProperty(PROP_ASM_BUFF_ANIM).toUInt64();
						auto* anim = CreateBuffAnimation(resid);
						m_Target->GetASM()->AddStateAnim(anim);
						});
					AnimationManager::GetInstance()->AddAnimation(anim);
				}
			}
		}
	}
}
void CastAction::Exit()
{

}

void CastAction::Enter()
{
	m_Target = m_Actor->GetTarget();
	if(m_Target){
		float degree = m_Actor->GetMoveDestAngle(m_Target->GetPos());
		int dir = m_Actor->GetDirByDegree(degree);
		dir = GMath::Dir8toDir4(dir);
		m_Actor->SetDir(dir);
		m_Target->SetDir(dir);
		m_Target->ReverseDir();
		m_Target->GetASM()->SetAction(ACTION_BATIDLE);
	}
	
	m_pASM->SetAction(ACTION_BATIDLE);
	m_Actor->SetProperty(PROP_COMBAT_ACTING, true);
}

void BeHitAction::Update()
{
	auto* avatar = m_pASM->GetAvatar();
	if (!avatar)return;
	avatar->Update();
	
	int action = m_pASM->GetActionID();
	if (action == ACTION_BEHIT) {
		if (avatar->IsGroupEndUpdate()) {
			if (m_Actor->GetProperty(PROP_IS_DEAD).toBool()) {
				DeadFlyAction* action = new DeadFlyAction(m_Actor, m_Attacker, MoveVec);
				m_pASM->ChangeAction(action);
			}
			else {
				m_Actor->SetActionID(ACTION_BATIDLE);
				m_pASM->RestoreAction();
			}
		}
		else if (avatar->IsFrameUpdate()) {
			if (avatar->CurrentFrame == 1) {
				Animation* anim = new Animation(m_Actor->GetProperty(PROP_ASM_BEHIT_ANIM).toUInt64());
				anim->SetLoop(1);
				anim->Offset.y = -avatar->GetFrameKeyY() + avatar->GetFrameHeight() / 2.0f;
				anim->AddFrameCallback(1, [this, avatar]() {
					Pos pos = m_Actor->GetPos();
					pos.x = pos.x + MoveVec.x * 3;
					pos.y = pos.y + MoveVec.y * 3;
					m_Actor->SetPos(pos);
				});
				anim->AddFrameCallback(anim->GroupFrameCount / 2, [this, avatar]() {
					m_Actor->GetMoveHandle()->MoveOnScreenWithDuration({ MoveVec.x * 7 ,MoveVec.y * 7 }, PERFRAME_TIME * 4, true);
				});
				m_Actor->GetASM()->AddStateAnim(anim);
				int tm = (int)(anim->GroupFrameCount*anim->FrameInterval * 1000);
				avatar->Pause(tm);
			}
		}
	}
}

void BeHitAction::Exit()
{
	m_Actor->SetPos(m_SavedPos);
	m_Actor->GetMoveHandle()->MoveOnScreenWithDuration({ -MoveVec.x * 10,-MoveVec.y * 10 }, PERFRAME_TIME / 2, true);
}

void BeHitAction::Enter()
{
	m_pASM->SetAction(ACTION_BEHIT);
	m_SavedPos = m_Actor->GetPos();
	auto*targetAvatar = m_pASM->GetAvatar();
	BeatNumber* beatNumber = new BeatNumber();
	auto* map = SCENE_MANAGER_INSTANCE->GetCurrentScene()->GetGameMap();
	int offx = map != nullptr ? map->GetMapOffsetX() : 0 ;
	int offy = map != nullptr ? map->GetMapOffsetY() : 0;
	if(m_Actor->IsCombat()){
		offx = offy = 0;
	}
	float bny = (float)(m_Actor->GetY() + offy - targetAvatar->GetFrameKeyY() + targetAvatar->GetFrameHeight() / 2);
	beatNumber->SetPos((float)m_Actor->GetX() + offx, bny);

	beatNumber->SetNumber(m_Actor->GetProperty(PROP_ASM_DAMAGE).toFloat());
	beatNumber->Beat();
	AnimationManager::GetInstance()->AddBeatNumber(beatNumber);
}

void BeHitAction::Draw()
{
	
}

void BeCastAction::Update()
{
	auto* avatar = m_pASM->GetAvatar();
	if (!avatar)return;
	avatar->Update();
	int action = m_pASM->GetActionID();
	if (action == ACTION_BEHIT) {
		if (avatar->IsGroupEndUpdate()) {
			if(m_Actor->GetProperty(PROP_IS_DEAD).toBool()){
				DeadFlyAction* action = new DeadFlyAction(m_Actor, m_Attacker, MoveVec);
				m_pASM->ChangeAction(action);
				m_pASM->GetAvatar()->Pause(100);
			}else{
				Pos pos = m_Actor->GetPos();
				pos.x = pos.x - MoveVec.x * 5;
				pos.y = pos.y - MoveVec.y * 5;
				m_Actor->SetPos(pos);
				m_pASM->SetAction(ACTION_BATIDLE);	
			}
		}
		else if (avatar->IsFrameUpdate()) {
			if (avatar->CurrentFrame == 1) {
				uint64_t id = m_Attacker->GetProperty(PROP_CAST_ID).toUInt64();
				Animation* anim = new Animation(id);
				anim->SetLoop(1);
				anim->Pos.x = avatar->Pos.x;
				anim->Pos.y = avatar->Pos.y - avatar->GetFrameKeyY() + avatar->GetFrameHeight() / 2;
				anim->AddFrameCallback(anim->GroupFrameCount * 5 / 6, [this, anim]() {
					Pos pos = m_Actor->GetPos();
					pos.x = pos.x + MoveVec.x * 5;
					pos.y = pos.y + MoveVec.y * 5;
					m_Actor->SetPos(pos);
				});

				AnimationManager::GetInstance()->AddAnimation(anim);
				int tm = (int)(anim->GroupFrameCount*anim->FrameInterval * 1000);
				avatar->Pause(tm);
			}
		}
	}
}

void BeCastAction::Exit()
{

}

void BeCastAction::Enter()
{
	m_pASM->SetAction(ACTION_BEHIT);

	auto*targetAvatar = m_pASM->GetAvatar();
	BeatNumber* beatNumber = new BeatNumber();
	auto* map = SCENE_MANAGER_INSTANCE->GetCurrentScene()->GetGameMap();
	int offx = map != nullptr ? map->GetMapOffsetX() : 0;
	int offy = map != nullptr ? map->GetMapOffsetY() : 0;
	if (m_Actor->IsCombat()) {
		offx = offy = 0;
	}
	float bny = (float)(m_Actor->GetY() + offy - targetAvatar->GetFrameKeyY() + targetAvatar->GetFrameHeight() / 2);
	beatNumber->SetPos((float)m_Actor->GetX() + offx, bny);
	
	beatNumber->SetNumber(m_Actor->GetProperty(PROP_ASM_DAMAGE).toFloat());
	beatNumber->Beat();
	AnimationManager::GetInstance()->AddBeatNumber(beatNumber);
}

void PathMoveAction::Update()
{
	auto* avatar = m_pASM->GetAvatar();
	if (!avatar)return;
	avatar->Update();
	int action = m_pASM->GetActionID();
	if (action == ACTION_IDLE) {
		if (avatar->IsFrameUpdate()) {
			if (m_Actor->GetMoveHandle()->IsMove()) {
				m_pASM->SetAction(ACTION_WALK);
			}
		}
	}
	else if (action == ACTION_WALK) {
		if (avatar->IsFrameUpdate()) {
			if (!m_Actor->GetMoveHandle()->IsMove()) {
				m_pASM->SetAction(ACTION_IDLE);
			}
		}
	}
}

void PathMoveAction::Exit()
{

}

void PathMoveAction::Enter()
{
	if (m_Actor->GetProperty(PROP_ACTION_ID).toInt() != ACTION_IDLE)
		m_Actor->SetActionID(ACTION_IDLE);
	m_pASM->SetAction(ACTION_IDLE);
}

void DeadFlyAction::Update()
{
	auto* avatar = m_pASM->GetAvatar();
	if (!avatar)return;
	avatar->Update();

	int dir = m_Actor->GetDir();
	Pos pos = m_Actor->GetPos();

	if (pos.y - avatar->GetFrameKeyY() <= 0) {
		m_Dir.y = -m_Dir.y;
	}

	if (pos.y - avatar->GetFrameKeyY() + avatar->GetFrameHeight() >= 600) {
		m_Dir.y = -m_Dir.y;
	}

	if (avatar->IsFrameUpdate()) {
		pos.x += m_Dir.x * 49;
		pos.y += m_Dir.y * 49;
		m_Actor->SetPos(pos);
	}
	
	if (m_Dir.x < 0) {
		if (pos.x - avatar->GetFrameKeyX() < 0 ) {
			Action* action = new Action(m_Actor);
			m_Actor->GetASM()->ChangeAction(action);
			m_Actor->SetPos(m_SavedPos);
			m_Actor->SetDir(m_SavedDir);
			m_Actor->SetActionID(ACTION_BATIDLE);
			return;
		}
	}
	else if (m_Dir.x >=0) {
		if (pos.x - avatar->GetFrameKeyX() + avatar->GetFrameWidth() >= 800) {
			Action* action = new Action(m_Actor);
			m_Actor->GetASM()->ChangeAction(action);
			m_Actor->SetPos(m_SavedPos);
			m_Actor->SetDir(m_SavedDir);
			m_Actor->SetActionID(ACTION_BATIDLE);
			return;
		}
	}

	if(avatar->IsGroupEndUpdate()){
		dir = GMath::NextDir4(dir);
		m_Actor->SetDir(dir);
	}
}

void DeadFlyAction::Enter()
{
	m_pASM->SetAction(ACTION_BEHIT);
	m_SavedPos = m_Actor->GetPos();
	auto* avatar = m_pASM->GetAvatar();
	m_SavedDir = avatar->GetDir();
	avatar->SetFrameInterval(0.016f * 4);
	avatar->SetLoop(0);
}


void DeadFlyAction::Exit()
{
	m_Actor->SetPos(m_SavedPos);
}

ActionStateMachine::ActionStateMachine(Actor* _actor)
	:m_Actor(_actor)
{
	m_TimeInterval = 0.016f * 5;
	m_pCurrentAction = nullptr;
	m_pPreviousAction = nullptr;
	for (int action = ACTION_IDLE; action < ACTION_COUNT; action++) {
		m_AvatarActions.insert({ action,nullptr });
		m_WeaponActions.insert({ action,nullptr });
	}
	m_PlayerShadow = new Animation(SHAPEWDF, 0xDCE4B562);
	m_WeaponID = m_Actor->GetProperty(PROP_WEAPON_AVATAR_ID).toString();
	m_AvatarID = m_Actor->GetProperty(PROP_AVATAR_ID).toString();
	m_ActionID = ACTION_IDLE;
	m_bMoveActionToBack = false;
	m_ActionQueue.push_back(ACTION_IDLE);
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

	SafeDelete(m_PlayerShadow);
	for (auto& it : m_StateAnimQueue){
		SafeDelete(it);
	}
	m_StateAnimQueue.clear();
}

void ActionStateMachine::Update()
{
	int msdt = (int)WINDOW_INSTANCE->GetDeltaTimeMilliseconds();
	for (auto& wrap : m_TimerFuncs) {
		wrap.ms -= msdt;
		if (wrap.ms <= 0) {
			wrap.func();
			wrap.markd = true;
		}
	}

	for (auto it = m_TimerFuncs.begin(); it != m_TimerFuncs.end();) {
		if(it->markd){
			it = m_TimerFuncs.erase(it);
		}else{
			it++;
		}
	}

	if (m_AvatarID != m_Actor->GetProperty(PROP_AVATAR_ID).toString())
	{
		SetAvatar(m_Actor->GetProperty(PROP_AVATAR_ID).toString());
	}
	if (m_HasWeapon != (m_Actor->GetWeaponAvatarID() != "")) {
		m_HasWeapon = (m_Actor->GetWeaponAvatarID() != "");
	}
	if (m_HasWeapon) {
		if (m_WeaponID != m_Actor->GetWeaponAvatarID()) {
			SetWeapon(m_Actor->GetWeaponAvatarID());
		}
	}

	if (m_bMoveActionToBack) {
		int action = m_ActionQueue.front();
		m_ActionQueue.pop_front();
		m_ActionQueue.push_back(action);
		m_bMoveActionToBack = false;
	}

	//auto* avatar = GetAvatar(m_ActionID);
	//if (!avatar)return;
	m_ActionID = -1;
	if (!m_ActionQueue.empty()) {
		m_ActionID = m_ActionQueue.front();
	}
	auto* avatar = GetAvatar(m_ActionID);
	if (!avatar)return;
	avatar->Update();
	if (avatar->GetState() == ANIMATION_STOP) {
		cxlog_info("pop action %d\n", m_ActionID);
		m_ActionQueue.pop_front();
	}

	/*if (m_pCurrentAction) {
		m_pCurrentAction->Update();
	}*/

	if (HasWeapon() && action_is_show_weapon(m_ActionID)) {
		auto* weapon = GetWeapon(m_ActionID);
		weapon->CurrentFrame = avatar->CurrentFrame;
	}

	for (auto& anim : m_StateAnimQueue) {
		anim->Update();
	}
	for (auto it = m_StateAnimQueue.begin(); it != m_StateAnimQueue.end();) {
		if ((*it)->GetState() == ANIMATION_STOP) {
			delete *it;
			it = m_StateAnimQueue.erase(it);
		}
		else {
			it++;
		}
	}
}

void ActionStateMachine::Draw()
{
	auto* avatar = GetAvatar(m_ActionID);
	if (!avatar)return;

	int screenWidth = WINDOW_INSTANCE->GetWidth();
	int screenHeight = WINDOW_INSTANCE->GetHeight();
	int halfScreenWidth = screenWidth / 2;
	int halfScreenHeight = screenHeight / 2;

	Pos pos = m_Actor->GetPos();
	int dir = m_Actor->GetDir();
	if (m_Actor->IsCombat()) {
		avatar->Pos.x = pos.x;
		avatar->Pos.y = pos.y;
	}else{
		if (m_Actor->IsLocal() && m_Actor->GetScene() != nullptr&&m_Actor->GetScene()->GetGameMap() != nullptr) {
			int mapWidth = m_Actor->GetScene()->GetGameMap()->GetWidth();
			int mapHeight = m_Actor->GetScene()->GetGameMap()->GetHeight();
			int px = m_Actor->GetX();
			int py = m_Actor->GetY();
			int maxMapOffsetX = mapWidth - halfScreenWidth;
			int maxMapOffsetY = mapHeight - halfScreenHeight;

			px = px < halfScreenWidth ? px :
				(px > maxMapOffsetX ?
				(screenWidth - (mapWidth - px)) : halfScreenWidth);
			py = py < halfScreenHeight ? py :
				(py > maxMapOffsetY ?
				(screenHeight - (mapHeight - py)) : halfScreenHeight);

			avatar->Pos.x = (float)px;
			avatar->Pos.y = (float)py;
		}
		else {
			if (m_Actor->GetScene() != nullptr&&m_Actor->GetScene()->GetGameMap() != nullptr) {
				int offx = m_Actor->GetScene()->GetGameMap()->GetMapOffsetX();
				int offy = m_Actor->GetScene()->GetGameMap()->GetMapOffsetY();
				avatar->Pos.x = (float)(m_Actor->GetX() + offx);
				avatar->Pos.y = (float)(m_Actor->GetY() + offy);
			}
			else {
				avatar->Pos.x = pos.x;
				avatar->Pos.y = pos.y;
			}
		}
	}

	avatar->Dir = dir;

	m_PlayerShadow->Pos.x = avatar->Pos.x;
	m_PlayerShadow->Pos.y = avatar->Pos.y;
	m_PlayerShadow->Draw();
	avatar->Draw();

	if (HasWeapon() && action_is_show_weapon(m_ActionID)) {
		auto* weapon = GetWeapon(m_ActionID);
		weapon->Pos.x = avatar->Pos.x;
		weapon->Pos.y = avatar->Pos.y;
		weapon->Dir = dir;
		weapon->Draw();
	}

	if (m_pCurrentAction) {
		m_pCurrentAction->Draw();
	}

	for (auto& anim : m_StateAnimQueue) {
		anim->Pos.x = avatar->Pos.x + anim->Offset.x;
		anim->Pos.y = avatar->Pos.y + anim->Offset.y;
		anim->Draw();
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
	if (!avatar)return;
	avatar->Reset();
	if (!HasWeapon() || !action_is_show_weapon(m_ActionID))return;
	auto* weapon = GetWeapon(m_ActionID);
	weapon->Reset();
}

void ActionStateMachine::RestoreAction()
{
	if (m_pCurrentAction) {
		m_pCurrentAction->OnExit();
		delete m_pCurrentAction;
	}
	m_pCurrentAction = m_pPreviousAction;
	if(m_pCurrentAction){
		m_pCurrentAction->OnEnter();
	}
	m_pPreviousAction = nullptr;
}

void ActionStateMachine::ChangeAction(Action* action)
{
	if (m_pCurrentAction) {
		m_pCurrentAction->OnExit();
	}
	if (m_pPreviousAction) {
		delete m_pPreviousAction;
		m_pPreviousAction = nullptr;
	}
	m_pPreviousAction = m_pCurrentAction;
	m_pCurrentAction = action;
	if (m_pCurrentAction) {
		m_pCurrentAction->OnEnter();
	}
}

void ActionStateMachine::EnsureLoadAction(int action)
{
	if (action < ACTION_IDLE || action >= ACTION_COUNT)return;

	if (m_AvatarActions.find(action) == m_AvatarActions.end()) {
		auto resid = RESOURCE_MANAGER_INSTANCE->GetActorActionResID(m_Actor->GetProperty(PROP_ACTOR_TYPE).toInt(), m_AvatarID, action);
		if (resid == 0)return;
		if (m_Actor->GetPalette().size() != 0) {
			m_AvatarActions[action] = new Animation(resid, &m_Actor->GetPalette());
		}
		else {
			m_AvatarActions[action] = new Animation(resid);
		}
		
		m_AvatarActions[action]->FrameInterval = m_TimeInterval;
		m_AvatarActions[action]->SetLoop(0);
	}

	if (m_HasWeapon) {
		if (m_WeaponActions.find(action) == m_WeaponActions.end()) {
			auto resid = RESOURCE_MANAGER_INSTANCE->GetActionResID(AVATAR_TYPE_WEAPON, m_WeaponID, action);
			if (resid == 0) {
				m_HasWeapon = false; 
				return;
			}
			m_WeaponActions[action] = new Animation(resid);
			m_WeaponActions[action]->FrameInterval = m_TimeInterval;
			m_WeaponActions[action]->SetLoop(0);
		}
	}
}

Animation* ActionStateMachine::GetAvatar(int action)
{
	if (action == -1)action = m_ActionID;
	EnsureLoadAction(action);
	return m_AvatarActions[action];
}

Animation* ActionStateMachine::GetWeapon(int action)
{
	if (action == -1)action = m_ActionID;
	EnsureLoadAction(action);
	return m_WeaponActions[action];
}

void ActionStateMachine::Reset()
{
	SetAvatar(m_Actor->GetProperty(PROP_AVATAR_ID).toString());
	SetWeapon(m_Actor->GetWeaponAvatarID());

	PathMoveAction* action = new PathMoveAction(m_Actor);
	ChangeAction(action);
}

int ActionStateMachine::GetDirCount(int action)
{
	if (action == -1)action = m_ActionID;
	if (m_AvatarActions[action]) {
		return m_AvatarActions[action]->GroupCount;
	}
	return 4;
}

void ActionStateMachine::AddStateAnim(Animation* anim)
{
	if (anim == nullptr)return;
	m_StateAnimQueue.push_back(anim);
}

void ActionStateMachine::RemoveStateAnim(Animation* anim)
{
	for (auto it = m_StateAnimQueue.begin(); it != m_StateAnimQueue.end();) {
		if ((*it) == anim) {
			delete (*it);
			m_StateAnimQueue.erase(it);
			break;
		}
	}
}

void ActionStateMachine::ClearStateAnim()
{
	for (auto& it : m_StateAnimQueue) {
		SafeDelete(it);
	}
	m_StateAnimQueue.clear();
}

void ActionStateMachine::AddDelayCallback(int ms, function<void()> func)
{
	TimerFuncWrap wrap;
	wrap.ms = ms;
	wrap.func = func;
	m_TimerFuncs.push_back(wrap);
}

void AttackAction2::Update()
{
	auto* avatar = m_pASM->GetAvatar();
	if (!avatar)return;
	avatar->Update();
}
void AttackAction2::Draw()
{

}
void AttackAction2::Exit()
{

}
void AttackAction2::Enter()
{
	m_pASM->SetAction(ACTION_BATIDLE);

	auto* avatar = m_pASM->GetAvatar();
	if (!avatar)return;

	avatar->AddCallback(avatar->GroupFrameCount*avatar->GetFrameInterval(), [this]() {
		m_pASM->SetAction(ACTION_RUNTO);
		auto* avatar = m_pASM->GetAvatar();
		if (!avatar)return;
		avatar->AddCallback(avatar->GroupFrameCount*avatar->GetFrameInterval(), [this]() {
			m_pASM->SetAction(ACTION_ATTACK);
			auto* avatar = m_pASM->GetAvatar();
			if (!avatar)return;
			avatar->AddCallback(avatar->GroupFrameCount*avatar->GetFrameInterval(), [this]() {
				m_pASM->SetAction(ACTION_RUNBACK);
				auto* avatar = m_pASM->GetAvatar();
				if (!avatar)return;
				avatar->AddCallback(avatar->GroupFrameCount*avatar->GetFrameInterval(), [this]() {
					m_pASM->SetAction(ACTION_BATIDLE);
					});
				});
			});
		});
}


#endif

int action_get_attack_key_frame(const char* str)
{
	return g_AttackKeyFrame[str];
}

int action_calc_run_to_pos(lua_State*L){
	Actor* actor = lua_check_actor(L, 1);
	Pos runto;
	//CalcRunToPos(actor, actor->GetTarget(), runto);
	lua_pushnumber(L, runto.x);
	lua_pushnumber(L, runto.y);
	return 2;
}

void luaopen_action(lua_State* L)
{
	script_system_register_function(L, action_get_attack_key_frame);
	script_system_register_luac_function(L, action_calc_run_to_pos);
}

