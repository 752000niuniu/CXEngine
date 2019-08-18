#pragma once

#include <string>
#include "pos.h"


class Actor;
class ActionStateMachine;
class BaseSprite;


std::string action_get_name(int i);
size_t action_get_size();
std::string action_system_get_action(int i);
bool action_is_show_weapon(int action);

bool action_is_emotion_action(int action);
bool action_is_battle_action(int action);


class Action
{
public:
	Action(Actor* actor);
	virtual ~Action() {};
	virtual void Enter();
	virtual void Exit() {};
	virtual  void Update();
protected:
	ActionStateMachine *  pASM;
	Actor* actor;
};

class AttackAction : public Action
{
public:
	AttackAction(Actor* actor);
	virtual ~AttackAction();
	virtual  void Update();
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

class IdleAction : public Action
{
public:
	IdleAction(Actor* actor) :Action(actor) {};
	virtual ~IdleAction() {};
};


class ActionStateMachine
{
public:
	ActionStateMachine(Actor* actor);
	~ActionStateMachine() {};
	void Update();
	void Draw();

	void SetWeapon(int id);
	void SetAvatar(int id);
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
	int m_WeaponID;
	int m_AvatarID;
	bool m_HasWeapon;
	Action* m_pPreviousAction;
	Action* m_pCurrentAction;
	std::map<int, BaseSprite*> m_WeaponActions;
	std::map<int, BaseSprite*> m_AvatarActions;
};

