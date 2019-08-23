#pragma once

#include <string>
#include "pos.h"
#include <list>

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
class Animation;
class Action
{
public:
	Action(Actor* actor);
	virtual ~Action() {};
	virtual void Enter();
	virtual void Exit() {};
	virtual  void Update();
	virtual  void Draw() {};
protected:
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
	AttackAction(Actor* actor) :Action(actor) {};
	virtual ~AttackAction() {};
	virtual  void Update();
	virtual  void Draw() ;
	void AddTarget(Actor* target);
	virtual void Exit();
	virtual void Enter();
	
private:
	Actor * m_Target;
	Pos m_BackupPos;
	int m_BackupActionID;
	float m_SavedVelocity;
	int m_ID;
	Pos m_Runto;
	Pos m_AttackVec;
};


class CastAction :public Action
{
public:
	CastAction(Actor* actor, Actor* target, uint32_t skill) :Action(actor), m_Target(target), m_Skill(skill) {};
	virtual ~CastAction() {};
	virtual  void Update();
	virtual void Exit();
	virtual void Enter();
private:
	Actor* m_Target;
	uint32_t m_Skill;
};


class BeHitAction : public Action
{
public:
	BeHitAction(Actor* actor, Actor* attacker) :Action(actor), m_Attacker(attacker) {};
	virtual ~BeHitAction() {};
	virtual  void Update();
	virtual void Exit();
	virtual void Enter();
	void Draw();
	Pos MoveVec;
private:
	Actor* m_Attacker;
	int m_State;
	Animation* m_DamageAnim;
};

class BeCastAction : public Action
{
public:
	BeCastAction(Actor* actor, Actor* attacker) :Action(actor), m_Attacker(attacker) {};
	virtual ~BeCastAction() {};
	virtual  void Update();
	virtual void Exit();
	virtual void Enter();
private:
	Actor* m_Attacker;
	int m_State;
};

class DeadFlyAction : public Action
{
public:
	DeadFlyAction(Actor* actor, Pos dir) :Action(actor), m_Dir(dir) {};
	virtual ~DeadFlyAction() {};
	virtual  void Update();
	virtual void Enter();
private:
	Pos m_Dir;
	Pos m_FlyPos;
	Pos m_SavedPos;
};


class PathMoveAction : public Action
{
public:
	PathMoveAction(Actor* actor) :Action(actor){};
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
};
#endif // !SIMPLE_SERVER


