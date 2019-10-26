#pragma once

#include <string>
#include "pos.h"
#include <list>
#include "define_types.h"

std::string action_get_name(int i);
size_t action_get_size();
std::string action_system_get_action(int i);
bool action_is_show_weapon(int action);
bool action_is_emotion_action(int action);
bool action_is_battle_action(int action);


#ifndef SIMPLE_SERVER
class Actor;
class ActionStateMachine;
class Animation;
class BeatNumber;
class Action
{
public:
	Action(Actor* actor);
	virtual ~Action() {};
	virtual void Enter();
	virtual void Exit() {};
	virtual  void Update();
	virtual  void Draw() {};

	void OnEnter();
	void OnExit();
	EASMActionType GetType() { return m_Type; }
protected:
	EASMActionType m_Type;
	ActionStateMachine *  m_pASM;
	Actor* m_Actor;
};

/*
attack animation sequence:
Batidle -> Runto -> Attack() -> Runback -> Batidle
Attack will trigger other actor to play behit action
Attack will do some action delay 

timeline------check hit------------delay done---------check death--------------------------------------------------------
Attacker:   attack delay	     attack continue       runback               
Defender:   behit effect           behitback          clps|behit fly

*/
class AttackAction : public Action
{
public:
	AttackAction(Actor* actor) :Action(actor) { m_Type = ASM_ATTACK_ACTION; m_bFinishAttack = false; };
	virtual ~AttackAction() {};
	virtual  void Update();
	virtual  void Draw() ;
	virtual void Exit();
	virtual void Enter();
private:
	
	Pos m_BackupPos;
	int m_BackupActionID;
	float m_SavedVelocity;
	int m_ID;
	Pos m_Runto;
	Pos m_AttackVec;
	int m_ComboCount;
	Actor* m_Target;
	bool m_bFinishAttack;
};


class AttackAction2 : public Action
{
public:
	AttackAction2(Actor* actor) :Action(actor) { m_Type = ASM_ATTACK_ACTION; };
	virtual ~AttackAction2() {};
	virtual  void Update();
	virtual  void Draw();
	virtual void Exit();
	virtual void Enter();
private:

	
};

class CastAction :public Action
{
public:
	CastAction(Actor* actor) :Action(actor){ m_Type = ASM_CAST_ACTION; };
	virtual ~CastAction() {};
	virtual  void Update();
	virtual void Exit();
	virtual void Enter();
private:
	Pos m_AttackVec;
	Actor* m_Target;
};


class BeHitAction : public Action
{
public:
	BeHitAction(Actor* actor, Actor* attacker) :Action(actor), m_Attacker(attacker) { m_Type = ASM_BEHIT_ACTION; };
	virtual ~BeHitAction() {};
	virtual  void Update();
	virtual void Exit();
	virtual void Enter();
	void Draw();
	Pos MoveVec;
private:
	Actor* m_Attacker;
	int m_State;
	Pos m_SavedPos;
	bool m_bMoveEnd;
};

class BeCastAction : public Action
{
public:
	BeCastAction(Actor* actor, Actor* attacker) :Action(actor), m_Attacker(attacker) { m_Type = ASM_BECAST_ACTION; };
	virtual ~BeCastAction() {};
	virtual  void Update();
	virtual void Exit();
	virtual void Enter();
	Pos MoveVec;
private:
	Actor* m_Attacker;
	int m_State;
};

class DeadFlyAction : public Action
{
public:
	DeadFlyAction(Actor* actor, Actor* attacker, Pos dir) :Action(actor), m_Attacker(attacker), m_Dir(dir) { m_Type = ASM_DEAD_FLY_ACTION; };
	virtual ~DeadFlyAction() {};
	virtual  void Update();
	virtual void Enter();
	virtual void Exit();
private:
	int m_SavedDir;
	Pos m_Dir;
	Pos m_FlyPos;
	Pos m_SavedPos;
	Actor* m_Attacker;
};


class PathMoveAction : public Action
{
public:
	PathMoveAction(Actor* actor) :Action(actor) { m_Type = ASM_PATH_MOVE_ACTION; };
	virtual ~PathMoveAction() {};
	virtual  void Update();
	virtual void Exit();
	virtual void Enter();
private:
	int m_State;
};

class ActionStateMachine
{
public:
	struct TimerFuncWrap
	{
		int ms;
		function<void()> func;
		bool markd = false;
	};

	ActionStateMachine(Actor* actor);
	~ActionStateMachine();
	void Update();
	void Draw();

	void SetWeapon(CXString id);
	void SetAvatar(CXString id);
	void SetAction(int id);

	void RestoreAction();
	void ChangeAction(Action* action);
	void EnsureLoadAction(int action);
	
	Animation* GetAvatar(int action = -1);
	Animation* GetWeapon(int action = -1);
	bool HasWeapon() { return m_HasWeapon; }
	Action* GetAction() { return m_pCurrentAction; }
	void SetTimeInterval(float ti) { m_TimeInterval = ti; }
	void Reset();
	int GetDirCount(int action = -1);

	int GetActionID() { return m_ActionID; };

	void AddStateAnim(Animation* anim);
	void RemoveStateAnim(Animation* anim);
	void ClearStateAnim();

	void AddDelayCallback(int ms, function<void()> func);

	void MoveActionToBack() { m_bMoveActionToBack = true; }
	void PushAction(int action) { m_ActionQueue.push_back(action); }
	void ClearAction() { m_ActionQueue.clear(); }
	int GetCurrentAction() { 
		if (m_ActionQueue.empty())return -1;
		return m_ActionQueue.front();
	}
	BeatNumber* GetBeatNumber();
private:
	Actor * m_Actor;
	float m_TimeInterval;
	int m_ActionID;
	CXString m_WeaponID;
	CXString m_AvatarID;
	bool m_HasWeapon;
	Action* m_pPreviousAction;
	Action* m_pCurrentAction;
	std::map<int, Animation*> m_WeaponActions;
	std::map<int, Animation*> m_AvatarActions;
	Animation* m_PlayerShadow;
	deque<TimerFuncWrap> m_TimerFuncs;
	deque<Animation*> m_StateAnimQueue;
	bool		m_bMoveActionToBack;
	deque<int> m_ActionQueue;
	BeatNumber* m_BeatNumber;
};
#endif // !SIMPLE_SERVER

int action_get_attack_key_frame(const char* str);

void luaopen_action(lua_State* L);