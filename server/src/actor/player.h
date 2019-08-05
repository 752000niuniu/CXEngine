#pragma once
#include "pos.h"
#include "scene/game_map.h"

enum EActorType
{
	ACTOR_TYPE_DEFAULT = 0,
	ACTOR_TYPE_PLAYER,
	ACTOR_TYPE_PET,
	ACTOR_TYPE_NPC
};

class Player
{
public:
	Player(int roleID = 0);
	virtual ~Player();

	void SetRoleID(int roleid) { m_RoleID = roleid; };
	int GetRoleID() { return m_RoleID; };

	void SetSceneID(int sceneid) { m_SceneID = sceneid; }
	int GetSceneID() {return  m_SceneID; }
	void SetDir(int dir) { m_Dir = dir; };
	int GetDir() { return m_Dir; };

	int GetDirByDegree(float degree);
	void ReverseDir();
	
	float GetMoveDestDistSquare(Pos dest);
	float GetMoveDestAngle(Pos dest);
	void SetActionID(int state) { m_ActionID = state; };
	int GetActionID() { return m_ActionID; }

	void SetWeaponID(int weapon) { m_WeaponID = weapon; }
	int GetWeaponID() { return m_WeaponID; }

	virtual void OnUpdate(float dt);
	void SetPos(float x, float y) { m_Pos.x = x; m_Pos.y = y; };
	void SetPos(Pos p) { m_Pos = p; };
	Pos GetPos() { return m_Pos; };
	Pos GetMoveToPos() { return m_MoveToPos; };

	void SetCombatPos(float x, float y) { m_CombatPos.x = x; m_CombatPos.y = y; };
	void SetCombatPos(Pos pos) { m_CombatPos = pos; };
	Pos GetCombatPos() { return m_CombatPos; };

	void SetCombatBackupPos(Pos pos) { m_CombatPosBackup = pos; };
	Pos GetCombatBackupPos() { return m_CombatPosBackup; };

	Pos GetCombatTargetPos() { return m_CombatTargetPos; };

	int GetX() { return static_cast<int>(m_Pos.x); }
	int GetY() { return  static_cast<int>(m_Pos.y); }

	void SetX(float x) { m_Pos.x = x; }
	void SetY(float y) { m_Pos.y = y; }

	void SetBox();
	void SetBoxX(int x) { m_Box.x = x; };
	void SetBoxY(int y) { m_Box.y = y; }
	int GetBoxX() { return static_cast<int>(m_Pos.x / 20); }
	int GetBoxY() { return static_cast<int>(m_Pos.y / 20); }


	void TranslateX(float x) { m_Pos.x += x; }
	void TranslateY(float y) { m_Pos.y += y; }

	void MoveTo(GameMap* gameMap, int destX, int destY);
	void MoveToRandom();
	void SetVelocity(float velocity) { m_MoveVelocity = velocity; };
	float GetVelocity() { return  m_MoveVelocity; };

	bool IsMove() { return m_IsMove; }
	void SetCombatTargetPos(Pos pos) { m_CombatTargetPos = pos; };

	void SetNickName(std::string name) { m_NickName = name; };
	std::string GetNickName() { return m_NickName; };
	void SetIsCombat(bool bcombat) { m_bInCombat = bcombat; }
	bool IsCombat() { return m_bInCombat; }

	void SetTargetID(int id) { m_TargetID = id; }
	int GetTargetID() { return m_TargetID; }

	std::list<Pos>& GetMoveList() { return m_MoveList; };
	std::list<Pos>& GetBackupMoveList() { return m_BackupMoveList; };

	bool IsAutoRun() { return m_IsAutoRun; };
	void SetAutoRun(bool autoRun) { m_IsAutoRun = autoRun; };
	void SetActorID(int id) { m_ActorID = id; };
	int GetActorID() { return m_ActorID; };
	void Say(std::wstring Text);
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

	void ChangeAction(int actionID) { m_ActionID = actionID; }
	void ChangeWeapon(int WeaponID) { m_WeaponID = WeaponID; }
	void ChangeRole(int roleID) { m_RoleID = roleID; }
protected:
	bool m_IsAutoRun;
	int m_ActorID;
	int m_RoleID;				//current role

	std::string m_NickName;

	int m_WeaponID;				//current weapon
	int m_ActionID;				//current action
	int m_ActorType;
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

	int m_SayDuration;

	int m_SceneID;
};



struct NpcTemplate
{
	uint32_t Pack;
	uint32_t SpriteID;

};

class Npc : public Player
{
public:

	Npc(const char* player_name, float x, float y, int dir, int role_id, int action_id, std::string msg);
	virtual ~Npc();
	
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


Player* actor_manager_create_player(int role_id,const char* name);
Player* actor_manager_find_player_by_name(const char* name);
void actor_manager_remove_player_by_name(const char* name);
void actor_manager_clear_player();

std::vector<Player*> actor_manager_fetch_all_players();