#pragma once

#include <string>
#include "actor.h"
#include "animation/sprite.h"

std::string action_get_name(int i);
size_t action_get_size();
std::string action_system_get_action(int i);
bool action_is_show_weapon(int action);
class Action
{
public:
	enum State
	{
		Idle = 0,
		Walk,
		Sit,
		Angry,
		Sayhi,
		Dance,
		Salute,
		Clps,
		Cry,
		Batidle,
		Attack,
		Cast,
		Behit,
		Runto,
		Runback,
		Defend,
		COUNT
	};

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
	int m_ID;
	ActionStateMachine* m_ASM;
	Actor* m_Actor;
};

class AttackAction : public Action
{
public:
	AttackAction(Actor* actor);
	virtual ~AttackAction() {};
	virtual BaseSprite* OnUpdate(BaseSprite* avatar);
	void AddTarget(Actor* target);
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




class Direction
{
public:
	enum Dir
	{
		N = 6,
		S = 4,
		W = 5,
		E = 7,
		N_E = 3,
		N_W = 2,
		S_E = 0,
		S_W = 1,
	};
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
	Actor* m_Actor;
	float m_TimeInterval;
	int m_WeaponID;
	int m_AvatarID;
	bool m_HasWeapon;
	Action* m_pPreviousAction;
	Action* m_pCurrentAction;

	std::map<int, BaseSprite*> m_WeaponActions;
	std::map<int, BaseSprite*> m_AvatarActions;

};
