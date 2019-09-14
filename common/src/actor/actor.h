#pragma once

#include "define_types.h"
#include "pos.h"
#include "entity.h"
#include <vector>

class MoveHandle;
#ifndef SIMPLE_SERVER
#include "ui.h"
#include <NESupport.h>
class ActionStateMachine;
class TextView;
#endif
#include "actor_prop.h"

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

	bool IsCombat() { return m_Props[PROP_IS_COMBAT].b; }

	void SetLocal(bool local) { m_IsLocalPlayer = local; }
	bool IsLocal();

	bool IsMove() { return m_IsMove; }

	float GetCombatDistSquare();
	float GetCombatAngle();
	float GetMoveDestDistSquare(Pos dest);
	float GetMoveDestAngle(Pos dest);

	void SetTurnReady(bool ready) { m_CombatProps.HasReady = ready; };
	bool IsTurnReady() { return m_CombatProps.HasReady; };
	BaseScene* GetScene();

	MoveHandle* GetMoveHandle() { return m_MoveHandle; }

	CXString GetWeaponAvatarID();
#ifndef SIMPLE_SERVER
	std::vector<NE::PalSchemePart>& GetPalette() { return m_PatMatrix; }
	void SetPalette(std::vector<NE::PalSchemePart> patMatrix) { m_PatMatrix = patMatrix; }
	ActionStateMachine* GetASM() { return m_ASM; };
	Bound GetViewBounds() override;
	bool CheckDrag(int x, int y) override;
	void OnDragMove(int x, int y)override;
	void Say(std::string Text);
#endif
	

	template<typename T>
	void CopyProperty(int index,T& out) {
		ActorProp& prop = m_Props[index];
		switch (prop.type)
		{
		case PROP_TYPE_BOOL:
			out = prop.b;
			break;
		case PROP_TYPE_INT:
			out = prop.i;
			break;
		case PROP_TYPE_UINT64:
			out = prop.l;
			break;
		case PROP_TYPE_FLOAT:
			out = prop.f;
			break;
		case PROP_TYPE_STR:
			out = prop.s;
			break;
		case PROP_TYPE_VEC2:
			out[0] = prop.v2[0];
			out[1] = prop.v2[1];
			break;
		}
	}

	ActorProp& GetProperty(int index) { return m_Props[index]; }
	void SetProperty(int index, ActorProp prop) { m_Props[index] = prop; }
	void RegProperty(int index, ActorProp& prop) { assert(index == m_Props.size()); m_Props.push_back(prop); }
protected:
	
	CXString m_AvatarID;
	CXString m_WeaponAvatarID;
	
	Pos m_Pos;
	Pos m_MoveToPos;

	int m_Dir;
	int m_DirCount;
	bool m_IsMove;
	float m_MoveVelocity;

	bool m_bCalcMoveList;
	int m_SayDuration;
	bool m_IsLocalPlayer;
	std::list<Pos> m_MoveList;
	std::list<Pos> m_BackupMoveList;
	
	MoveHandle* m_MoveHandle;
	ActorCombatProps m_CombatProps;
	std::vector<NE::PalSchemePart> m_PatMatrix;
	std::vector<ActorProp> m_Props;

#ifndef SIMPLE_SERVER
	TextView* m_SayWidget;
	ActionStateMachine* m_ASM;
#endif
};

Actor* lua_check_actor(lua_State*L, int index);
void lua_push_actor(lua_State*L, Actor* actor);
void luaopen_actor(lua_State* L);
