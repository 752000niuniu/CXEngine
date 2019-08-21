#pragma once

#include <string>
#include "pos.h"



std::string action_get_name(int i);
size_t action_get_size();
std::string action_system_get_action(int i);
bool action_is_show_weapon(int action);

bool action_is_emotion_action(int action);
bool action_is_battle_action(int action);



#ifndef SIMPLE_SERVER
class Actor;
class ActionStateMachine;
class BaseSprite;
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
	ActionStateMachine *  pASM;
	Actor* actor;
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
	AttackAction(Actor* actor);
	virtual ~AttackAction();
	virtual  void Update();
	virtual  void Draw() ;
	void AddTarget(Actor* target);
	virtual void Exit();
	virtual void Enter();
	
private:
	Actor * m_Target;
	Pos m_BackupPos;
	int m_BackupActionID;
	float m_Velocity;
	int m_ID;

};

class BeHitAction : public Action
{
public:
	BeHitAction(Actor* actor, Actor* attacker);
	virtual ~BeHitAction();
	virtual  void Update();
	virtual void Exit();
	virtual void Enter();
	void Draw();
private:
	Actor* m_Attacker;
	int m_State;
	Animation* m_BehitAnim;
	Animation* m_DamageAnim;
};

class IdleAction : public Action
{
public:
	IdleAction(Actor* actor) :Action(actor) { };
	virtual ~IdleAction() {};
	void Enter();
	
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

	BaseSprite* GetAvatar(int action = -1);
	BaseSprite* GetWeapon(int action = -1);
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
	std::map<int, BaseSprite*> m_WeaponActions;
	std::map<int, BaseSprite*> m_AvatarActions;
};
#endif // !SIMPLE_SERVER


