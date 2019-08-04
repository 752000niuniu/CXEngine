#pragma once


#include "pos.h"
#include "entity.h"
#include "action.h"
#include "actor.h"
#include "scene/game_map.h"
#include "animation/frame_animation.h"
#include "state.h"
#include "ui.h"

class Player : public Actor, public BaseGameEntity
{
public:

	Player(int roleID = 0);
	virtual ~Player();

	void ClearFrames();
	void ReloadFrames();
	void ResetDirAll(int dir);
	void ResetDir(int dir);
	

	int GetDirByDegree(float degree);
	void ReverseDir();
	
	virtual void OnUpdate(float dt);
	virtual void OnDraw(GameMap* gameMapPtr);
	virtual void OnDraw(int px, int py);
	
	float GetCombatDistSquare();
	float GetCombatAngle();
	float GetMoveDestDistSquare(Pos dest);
	float GetMoveDestAngle(Pos dest);

	void MoveTo(float x, float y);

	void MoveTo(GameMap* gameMapPtr, int destX, int destY);
	void MoveToRandom();
	
	int GetDrawY();

	FrameAnimation* GetPlayerFrame(int actionID);
	FrameAnimation* GetCurrentPlayerFrame();
	FrameAnimation* GetCurrentWeaponFrame();

	bool IsMove() { return m_IsMove; }
	void SaveFrame(int index);
	void ChangeRole(int roleID);
	void ChangeWeapon(int WeaponID);
	void ChangeAction(int actionID);

	void SetSkillFrame(FrameAnimation* anim);
	FrameAnimation& GetSkillFrame() { return *m_SkillFrame; }

	StateMachine<Player>* GetFSM() { return m_pFSM; };

	bool HandleMessage(const Telegram& msg) override;
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
	void LoadRoleFrame();
	void LoadWeaponFrame();
	void SyncRoleWeaponFrame();


	StateMachine<Player>* m_pFSM;

	std::map<int, FrameAnimation*> m_PlayerFrames;
	std::map<int, FrameAnimation*> m_WeaponFrames;
	FrameAnimation* m_SkillFrame;

	std::list<Pos> m_MoveList;
	std::list<Pos> m_BackupMoveList;


	TextView* m_SayWidget;
	std::vector<Player*> m_Targets;
};


struct NpcTemplate
{
	uint32_t Pack;
	uint32_t SpriteID;

};

class Npc : public Player, public View
{
public:
	Npc(int role_id) :Player(role_id) {};
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
	Pet(int role_id) :Player(role_id) { m_ActorType = ACTOR_TYPE_PET; };
	Pet(const char* player_name, float x, float y, int dir, int role_id, int action_id);
	virtual void OnDraw(int px, int py);
	
	virtual ~Pet();
};

