#pragma once


#include "pos.h"
#include "entity.h"
#include "actor/actor.h"
#include "state.h"
#include "ui.h"

class TextView;
class GameMap;
class FrameAnimation;

class Player : public Actor 
{
public:
	Player(uint64_t pid);
	virtual ~Player();

	void ResetDirAll(int dir);
	void ResetDir(int dir);

	virtual void OnUpdate();
	virtual void OnDraw(GameMap* gameMapPtr);

	int GetDrawY();

	void SaveFrame(int index);

	void LogInfo();

	void Say(std::wstring Text);
	void Say(std::string Text);

	void AddTarget(Player* target) { m_Targets.push_back(target); }
	std::vector<Player*> GetTargets() { return m_Targets; }
	void ClearTargets() { m_Targets.clear(); }

	Bound GetScreenBound();

	void SetDir(int dir) override;
	void SetAction(int state);
protected:
	
	std::vector<Player*> m_Targets;
	TextView* m_SayWidget;
};


struct NpcTemplate
{
	uint32_t Pack;
	uint32_t SpriteID;
};

class Npc : public Player
{
public:
	Npc(uint64_t id) :Player(id) {};
	Npc(const char* player_name, float x, float y, int dir, int role_id, int action_id, std::string msg);
	virtual ~Npc();
	virtual void OnDraw(GameMap* m_GameMapPtr) override;

	bool HandleMessage(const Telegram& msg);

	void OnClick(int button, int x, int y) override;
	
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
	Pet(uint64_t id) :Player(id) { m_ActorType = ACTOR_TYPE_PET; };
	Pet(const char* player_name, float x, float y, int dir, int role_id, int action_id);
	virtual void OnDraw(int px, int py);

	virtual ~Pet();
};


