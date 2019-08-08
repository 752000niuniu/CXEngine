#pragma once


#include "pos.h"
#include "entity.h"
#include "actor/action.h"
#include "actor/actor.h"
#include "scene/game_map.h"
#include "state.h"

class Player : public Actor
{
public:

	Player(uint64_t id = 0);
	virtual ~Player();

	void ResetDirAll(int dir);
	void ResetDir(int dir);


	int GetDirByDegree(float degree);
	void ReverseDir();

	virtual void OnUpdate(float dt);


	void MoveTo(float x, float y);

	void MoveTo(GameMap* gameMapPtr, int destX, int destY);
	void MoveToRandom();

	int GetDrawY();

	bool IsMove() { return m_IsMove; }
	void SaveFrame(int index);
	void ChangeRole(int roleID);
	void ChangeWeapon(int WeaponID);
	void ChangeAction(int actionID);

	

	std::list<Pos>& GetMoveList() { return m_MoveList; };
	std::list<Pos>& GetBackupMoveList() { return m_BackupMoveList; };
	void LogInfo();

	void Say(std::wstring Text);
	void Say(std::string Text);
	bool CanMove();

	void AddTarget(Player* target) { m_Targets.push_back(target); }
	std::vector<Player*> GetTargets() { return m_Targets; }
	void ClearTargets() { m_Targets.clear(); }
	float GetWidth();
	float GetHeight();

	Bound GetScreenBound();

	void SetBox();
	void SetDir(int dir);
	void SetActionID(int state);


protected:

	std::list<Pos> m_MoveList;
	std::list<Pos> m_BackupMoveList;
	std::vector<Player*> m_Targets;

};

