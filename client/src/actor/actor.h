#pragma once

#include "define_types.h"
#include "pos.h"
#include "scene/game_map.h"


enum EActorType
{
	ACTOR_TYPE_DEFAULT = 0,
	ACTOR_TYPE_PLAYER,
	ACTOR_TYPE_PET,
	ACTOR_TYPE_NPC
};

class Actor 
{
public:
	
	Actor(int roleID = 0);
	
	virtual ~Actor();
	virtual void OnUpdate(float dt) {};
	virtual void OnDraw(GameMap* gameMapPtr) {};
	virtual void OnDraw(int x, int y) {};

	void SetDir(int dir) { m_Dir = dir; };
	int GetDir() { return m_Dir; };

	void SetActionID(int state) { m_ActionID = state; };
	int GetActionID() { return m_ActionID; }

	void SetWeaponID(int weapon) { m_WeaponID = weapon; }
	int GetWeaponID() { return m_WeaponID; }

	void SetPos(float x, float y) { m_Pos.x = x; m_Pos.y = y; };
	void SetPos(Pos p) { m_Pos = p; };
	Pos GetPos() { return m_Pos; };

	Pos GetMoveToPos() { return m_MoveToPos; };

	void SetCombatPos(float x, float y) { m_CombatPos.x = x; m_CombatPos.y = y; };
	void SetCombatPos(Pos pos) { m_CombatPos = pos; };
	Pos GetCombatPos() { return m_CombatPos; };

	void SetCombatBackupPos(Pos pos) { m_CombatPosBackup = pos; };
	Pos GetCombatBackupPos() { return m_CombatPosBackup; };

	void SetCombatTargetPos(Pos pos) { m_CombatTargetPos = pos; };
	Pos GetCombatTargetPos() { return m_CombatTargetPos; };
	
	void SetBoxX(int x) { m_Box.x = x; };
	void SetBoxY(int y) { m_Box.y = y; }
	int GetBoxX() { return static_cast<int>(m_Pos.x / 20); }
	int GetBoxY() { return static_cast<int>(m_Pos.y / 20); }

	int GetX() { return static_cast<int>(m_Pos.x); }
	int GetY() { return  static_cast<int>(m_Pos.y); }
	void SetX(float x) { m_Pos.x = x; }
	void SetY(float y) { m_Pos.y = y; }

	void TranslateX(float x) { m_Pos.x += x; }
	void TranslateY(float y) { m_Pos.y += y; }

	void SetNickName(std::string name) { m_NickName = name; };
	std::string GetNickName() { return m_NickName; };

	void SetIsCombat(bool bcombat) { m_bInCombat = bcombat; }
	bool IsCombat() { return m_bInCombat; }

	void SetVelocity(float velocity) { m_MoveVelocity = velocity; };
	float GetVelocity() { return  m_MoveVelocity; };

	void SetTargetID(int id) { m_TargetID = id; }
	int GetTargetID() { return m_TargetID; }

	bool IsAutoRun() { return m_IsAutoRun; };
	void SetAutoRun(bool autoRun) { m_IsAutoRun = autoRun; };

	void SetActorID(int id) { m_ActorID = id; };
	int GetActorID() { return m_ActorID; };

	void SetHP(float hp) { m_HP = hp; }
	float GetHP() { return m_HP; }
	void AddHP(float hp) { m_HP += hp; }

	void SetMP(float mp) { m_MP = mp; }
	float GetMP() { return m_MP; }
	void AddMP(float mp) { m_MP += mp; }

	void SetLocalPlayer(bool local) { m_IsLocalPlayer = local; }
	bool IsLocalPlayer() { return m_IsLocalPlayer; }

	float GetFrameSpeed() { return m_FrameSpeed; };
	void SetFrameSpeed(float frame_speed) { m_FrameSpeed = frame_speed; };

	int GetType() { return m_ActorType; }
protected:
	float m_X;
	float m_Y;

	int m_RoleID;				//current role
	int m_ActorID;
	bool m_IsAutoRun;

	float m_FrameSpeed;
	int m_ActorType;
	std::string m_NickName;
	// bool m_HasWeapon;

	int m_WeaponID;				//current weapon
	int m_ActionID;				//current action

	Pos m_Pos;
	Pos m_MoveToPos;

	BoxPos m_Box;

	int m_Dir;
	int m_DirCount;
	bool m_IsMove;
	float m_MoveVelocity;

	bool m_bInCombat;
	Pos m_CombatPos;
	Pos m_CombatPosBackup;
	Pos m_CombatTargetPos;
	int m_TargetID;

	bool m_bSkillFrameShow;

	bool m_bCalcMoveList;
	float m_HP;
	float m_MP;
	int m_SayDuration;
	bool m_IsLocalPlayer;
};

void lua_push_actor(lua_State*L, Actor* actor);
void luaopen_actor(lua_State* L);
