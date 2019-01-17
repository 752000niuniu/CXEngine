#pragma once

#include "defines.h"
#include "Pos.h"
#include "../scene/GameMap.h"
#include "tsv.h"
#include "Logger.h"
#include "../animation/animation_database.h"
#include "../animation/FrameAnimation.h"
#include "../State.h"
#include "ui.h"
#include "ResourceManager.h"

#include "action.h"

enum EActorType
{
	ACTOR_TYPE_DEFAULT = 0,
	ACTOR_TYPE_PLAYER,
	ACTOR_TYPE_PET,
	ACTOR_TYPE_NPC
};

class Player : public BaseGameEntity
{
public:

	Player(int roleID = 0);
	virtual ~Player();

	void ClearFrames();
	void ReloadFrames();
	void ResetDirAll(int dir);
	void ResetDir(int dir);
	void SetDir(int dir);
	int GetDir() { return m_Dir; };

	int GetDirByDegree(float degree);
	void ReverseDir();
	float GetCombatDistSquare();
	float GetCombatAngle();
	float GetMoveDestDistSquare(Pos dest);
	float GetMoveDestAngle(Pos dest);
	void SetActionID(int state);
	int GetActionID() { return m_ActionID; }

	void SetWeaponID(int weapon) { m_WeaponID = weapon; }
	int GetWeaponID() { return m_WeaponID; }

	virtual void OnUpdate(float dt);
	virtual void OnDraw(GameMap* gameMapPtr);
	virtual void OnDraw(int px, int py);
	void SetPos(float x, float y);
	void SetPos(Pos p) { m_Pos = p; };
	Pos GetPos() { return m_Pos; };
	Pos GetMoveToPos() { return m_MoveToPos; };

	void SetCombatPos(float x, float y) { m_CombatPos.x = x; m_CombatPos.y = y; };
	void SetCombatPos(Pos pos) { m_CombatPos = pos; };
	Pos GetCombatPos() { return m_CombatPos; };

	void SetCombatBackupPos(Pos pos) { m_CombatPosBackup = pos; };
	Pos GetCombatBackupPos() { return m_CombatPosBackup; };

	Pos GetCombatTargetPos() { return m_CombatTargetPos; };

	void SetBox();
	void SetBoxX(int x) { m_Box.x = x; };
	void SetBoxY(int y) { m_Box.y = y; }
	int GetX() { return static_cast<int>(m_Pos.x); }
	int GetY() { return  static_cast<int>(m_Pos.y); }

	int GetBoxX() { return static_cast<int>(m_Pos.x / 20); }
	int GetBoxY() { return static_cast<int>(m_Pos.y / 20); }

	void SetX(float x) { m_Pos.x = x; }
	void SetY(float y) { m_Pos.y = y; }

	void TranslateX(float x) { m_Pos.x += x; }
	void TranslateY(float y) { m_Pos.y += y; }

	void MoveTo(float x, float y);

	void MoveTo(GameMap* gameMapPtr, int destX, int destY);
	void MoveToRandom();
	void SetVelocity(float velocity) { m_MoveVelocity = velocity; };
	float GetVelocity() { return  m_MoveVelocity; };
	int GetDrawY();

	FrameAnimation* GetPlayerFrame(int actionID);
	FrameAnimation* GetCurrentPlayerFrame();
	FrameAnimation* GetCurrentWeaponFrame();

	bool IsMove() { return m_IsMove; }
	void SaveFrame(int index);
	void ChangeRole(int roleID);
	void ChangeWeapon(int WeaponID);
	void ChangeAction(int actionID);
	void SetCombatTargetPos(Pos pos) { m_CombatTargetPos = pos; };

	void SetNickName(std::string name) { m_NickName = name; };
	std::string GetNickName() { return m_NickName; };
	void SetIsCombat(bool bcombat) { m_bInCombat = bcombat; }
	bool IsCombat() { return m_bInCombat; }

	void SetSkillFrame(FrameAnimation* anim);
	FrameAnimation& GetSkillFrame() { return *m_SkillFrame; }
	void SetTargetID(int id) { m_TargetID = id; }
	int GetTargetID() { return m_TargetID; }

	StateMachine<Player>* GetFSM() { return m_pFSM; };

	bool HandleMessage(const Telegram& msg) override;
	std::list<Pos>& GetMoveList() { return m_MoveList; };
	std::list<Pos>& GetBackupMoveList() { return m_BackupMoveList; };
	void LogInfo();

	bool IsAutoRun() { return m_IsAutoRun; };
	void SetAutoRun(bool autoRun) { m_IsAutoRun = autoRun; };
	void SetActorID(int id) { m_ActorID = id; };
	int GetActorID() { return m_ActorID; };
	void Say(std::wstring Text);
	void Say(std::string Text);
	bool CanMove();
	void SetHP(float hp) { m_HP = hp; }
	float GetHP() { return m_HP; }
	void AddHP(float hp) { m_HP += hp; }

	void SetMP(float mp) { m_MP = mp; }
	float GetMP() { return m_MP; }
	void AddMP(float mp) { m_MP += mp; }


	void AddTarget(Player* target) { m_Targets.push_back(target); }
	std::vector<Player*> GetTargets() { return m_Targets; }
	void ClearTargets() { m_Targets.clear(); }
	float GetWidth();
	float GetHeight();

	Bound GetScreenBound();

	void SetLocalPlayer(bool local) { m_IsLocalPlayer = local; }
	bool IsLocalPlayer() { return m_IsLocalPlayer; }
protected:
	void LoadRoleFrame();
	void LoadWeaponFrame();
	void SyncRoleWeaponFrame();
	bool m_IsAutoRun;
	int m_ActorID;
	int m_RoleID;				//current role

	int m_ActorType;
	std::string m_NickName;
	// bool m_HasWeapon;

	int m_WeaponID;				//current weapon
	int m_ActionID;				//current action

	StateMachine<Player>* m_pFSM;

	std::map<int, FrameAnimation*> m_PlayerFrames;
	std::map<int, FrameAnimation*> m_WeaponFrames;
	FrameAnimation* m_SkillFrame;

	Pos m_Pos;
	Pos m_MoveToPos;

	BoxPos m_Box;
	std::list<Pos> m_MoveList;
	std::list<Pos> m_BackupMoveList;

	int m_Dir;

	int m_DirCount;

	bool m_IsMove;
	float m_MoveVelocity;

	bool m_bInCombat;
	Pos m_CombatPos;
	Pos m_CombatPosBackup;
	Pos m_CombatTargetPos;
	int m_TargetID;

	std::vector<Player*> m_Targets;

	bool m_bSkillFrameShow;

	bool m_bCalcMoveList;
	float m_HP;
	float m_MP;

	TextView* m_SayWidget;
	int m_SayDuration;

	bool m_IsLocalPlayer;
};


struct NpcTemplate
{
	uint32_t Pack;
	uint32_t SpriteID;

};

class Npc : public Player, public View
{
public:

	Npc(const char* player_name, float x, float y, int dir, int role_id, int action_id, std::string msg);
	virtual ~Npc();
	virtual void OnDraw(GameMap* m_GameMapPtr) override;

	bool HandleMessage(const Telegram& msg);

	void OnClick(int button, int x, int y) override;
	Bound GetViewBounds()  override;
	int GetViewLayer()const override;
	void OnGlobleClick(int x, int y) override;
	void ShowDialog(std::string msg);
private:
	bool m_ShowDialog;
	std::string m_DialogContent;
	int m_Layer;
};

class Pet : public Player
{
public:
	Pet(const char* player_name, float x, float y, int dir, int role_id, int action_id);
	virtual ~Pet();
};

void lua_push_actor(lua_State*L, Player* actor);
void luaopen_actor(lua_State* L);