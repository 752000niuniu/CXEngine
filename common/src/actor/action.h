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
class Action
{
public:
	Action(Actor* actor);
	virtual ~Action() {};
	virtual BaseSprite*  OnUpdate(BaseSprite* avatar);
	virtual void Enter() {};
	virtual void Exit() {};

	void Update();
	void Draw();
	int GetID() { return m_ID; }
	void SetID(int id);
protected:
	float m_Velocity;
	int m_ID;
	ActionStateMachine* m_ASM;
	Actor* m_Actor;
};

class AttackAction : public Action
{
public:
	AttackAction(Actor* actor);
	virtual ~AttackAction();
	virtual BaseSprite* OnUpdate(BaseSprite* avatar);
	void AddTarget(Actor* target);
	virtual void Exit();
	virtual void Enter();
private:
	Actor * m_Target;
	Pos m_BackupPos;
};

class MoveAction : public Action
{
public:
	MoveAction(Actor* actor);
	virtual ~MoveAction() {};
	virtual BaseSprite* OnUpdate(BaseSprite* avatar);
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

	void RestoreAction();
	void ChangeAction(Action* action);

	void EnsureLoadAction(int action);
	BaseSprite* GetAvatar(int action);
	BaseSprite* GetWeapon(int action);
	bool HasWeapon() { return m_HasWeapon; }
	Action* GetAction() { return m_pCurrentAction; }
	void SetTimeInterval(float ti) { m_TimeInterval = ti; }
	void Reset();
	int GetDirCount(int action);
private:
	Actor * m_Actor;
	float m_TimeInterval;
	int m_WeaponID;
	int m_AvatarID;
	bool m_HasWeapon;
	Action* m_pPreviousAction;
	Action* m_pCurrentAction;
	std::map<int, BaseSprite*> m_WeaponActions;
	std::map<int, BaseSprite*> m_AvatarActions;
};
