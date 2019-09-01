#pragma once

#include "define_types.h"
#include "pos.h"
#include "entity.h"


class MoveHandle;
#ifndef SIMPLE_SERVER
#include "ui.h"
class ActionStateMachine;
class TextView;
#endif
class GameMap;

class BaseScene;

class ActorCombatProps
{
public:
	using CombatPos = Pos;

	CombatPos  Pos;
	int GroupType; //self/enemy
	CombatPos PosBackup;
	CombatPos TargetPos;
	bool HasReady;
};

#ifndef SIMPLE_SERVER
class Actor : public BaseGameEntity, public View
#else
class Actor : public BaseGameEntity
#endif
{
public:
	Actor(uint64_t id);

	virtual ~Actor();
	virtual void OnUpdate();
	virtual void OnDraw(GameMap* gameMapPtr) {};
	virtual void OnDraw(int x, int y) {};
	virtual void OnDraw();
	virtual bool HandleMessage(const Telegram& msg) { return false; };

	virtual void SetDir(int dir);
	int GetDir();
	int GetDirByDegree(float degree);
	void ReverseDir();

	void SetActionID(int state);
	int GetActionID() { return m_ActionID; }

	void SetSceneID(int id) { m_SceneID = id; };
	int GetSceneID() { return m_SceneID; };

	void SetRoleID(int id);
	int GetRoleID() { return m_RoleID; };

	void SetWeaponID(int weapon);
	int GetWeaponID() { return m_WeaponID; }


	void SetPos(float x, float y);
	void SetPos(Pos p);
	Pos GetPos() { return IsCombat() ? m_CombatProps.Pos : m_Pos; };

	void SetMoveToPos(Pos dest);
	Pos GetMoveToPos() { return m_MoveToPos; };

	void SetCombatBackupPos(Pos pos) { m_CombatProps.PosBackup = pos; };
	Pos GetCombatBackupPos() { return m_CombatProps.PosBackup; };

	void SetCombatTargetPos(Pos pos) { m_CombatProps.TargetPos = pos; };
	Pos GetCombatTargetPos() { return m_CombatProps.TargetPos; };

	int GetBoxX() { return static_cast<int>(GetPos().x / 20); }
	int GetBoxY() { return static_cast<int>(GetPos().y / 20); }

	int GetX() { return static_cast<int>(GetPos().x); }
	int GetY() { return static_cast<int>(GetPos().y); }

	void SetX(float x) { (IsCombat() ? m_CombatProps.Pos : m_Pos).x = x; }
	void SetY(float y) { (IsCombat() ? m_CombatProps.Pos : m_Pos).y = y; }

	void TranslateX(float x) { m_Pos.x += x; }
	void TranslateY(float y) { m_Pos.y += y; }

	virtual float GetWidth();
	virtual float GetHeight();

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

	void SetLocal(bool local) { m_IsLocalPlayer = local; }
	bool IsLocal();

	float GetFrameSpeed() { return m_FrameSpeed; };
	void SetFrameSpeed(float frame_speed) { m_FrameSpeed = frame_speed; };

	void SetType(int type) { m_ActorType = type; }
	int GetType() { return m_ActorType; }
	bool IsMove() { return m_IsMove; }

	float GetCombatDistSquare();
	float GetCombatAngle();
	float GetMoveDestDistSquare(Pos dest);
	float GetMoveDestAngle(Pos dest);

	void SetTurnReady(bool ready) { m_CombatProps.HasReady = ready; };
	bool IsTurnReady() { return m_CombatProps.HasReady; };
	BaseScene* GetScene();
	
	MoveHandle* GetMoveHandle() { return m_MoveHandle; }

	CXString GetAvatarID() { return m_AvatarID; }
	void SetAvatarID(CXString id) { m_AvatarID = id; }
	CXString GetWeaponAvatarID();
	void SetWeaponAvatarID(CXString id) { m_WeaponAvatarID = id; }

	uint64_t GetCastID() { return m_CastID; };
	void SetCastID(uint64_t id) { m_CastID = id; }

	bool IsDead() {return m_bDead;}
	void SetDead(bool dead) { m_bDead = dead; }
#ifndef SIMPLE_SERVER
	ActionStateMachine* GetASM() { return m_ASM; };
	Bound GetViewBounds() override;
	bool CheckDrag(int x, int y) override;
	void OnDragMove(int x, int y)override;
	void Say(std::string Text);
#endif
protected:
	int m_ActorID;
	bool m_IsAutoRun;
	bool m_bDead;
	float m_FrameSpeed;
	int m_ActorType;
	std::string m_NickName;

	CXString m_AvatarID;
	CXString m_WeaponAvatarID;
	int m_SceneID;
	int m_RoleID;			
	int m_WeaponID;
	int m_WeaponType;
	int m_ActionID;			
	
	Pos m_Pos;
	Pos m_MoveToPos;

	int m_Dir;
	int m_DirCount;
	bool m_IsMove;
	float m_MoveVelocity;
	bool m_bInCombat;	
	int m_TargetID;
	uint64_t m_CastID;

	bool m_bSkillFrameShow;

	bool m_bCalcMoveList;
	float m_HP;
	float m_MP;
	int m_SayDuration;
	bool m_IsLocalPlayer;

	std::list<Pos> m_MoveList;
	std::list<Pos> m_BackupMoveList;
	
	MoveHandle* m_MoveHandle;
	ActorCombatProps m_CombatProps;
#ifndef SIMPLE_SERVER
	TextView* m_SayWidget;
	ActionStateMachine* m_ASM;
#endif
};

Actor* lua_check_actor(lua_State*L, int index);
void lua_push_actor(lua_State*L, Actor* actor);
void luaopen_actor(lua_State* L);
