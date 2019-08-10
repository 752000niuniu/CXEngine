#pragma once


#include "define_types.h"
#include "actor/player.h"
#include "singleton.h"


/*
CombatSystem 战斗管理类
1. 添加Actors self/enemy
2. 战斗指令队列，接收战斗指令，给战斗指令排序
3. 回合战斗状态机 战斗开始，回合准备（接收全部战斗指令），回合进行（播放战斗指令），战斗结束
*/


enum ETurnCommandType
{
	TURN_COMMAND_ATTACK = 0,
	TURN_COMMAND_CAST
};

class TurnCommand
{
public:
	TurnCommand();
	~TurnCommand();
	virtual void Update() {};
	virtual void Draw() {};
	bool Finished() { return m_Finished; };
private:
	Actor * m_Master;
	std::set<Actor*> m_Targets;
	int m_Type;
	int m_Turn;
	bool m_Finished;
};

class CombatProp
{
public:
	int GroupType; //self/enemy
	float X;
	float Y;


};

class CombatSystem : public Singleton<CombatSystem>
{
public:
	enum State
	{
		BATTLE_DEFAULT = 0,
		BATTLE_START ,
		BATTLE_TURN_STAND_BY,
		BTTALE_TURN_EXECUTE,
		BTTALE_TURN_NEXT,
		BATTLE_END
	};

	CombatSystem();
	~CombatSystem();
	
	void AddActor(Actor* actor);
	void Update();
	void Draw();
	void NextTurn();
	void StartBattle();
	void EndBattle();
	
private:
	std::vector<Actor*> m_Actors;
	std::deque<TurnCommand> m_Commands;
	std::string m_CombatBGPath;
	int m_TurnCounter;
	int m_BattleState;
	int m_ActorReadyCounter;
	int m_ActorPlayCounter;
};


#define COMBAT_SYSTEM_INSTANCE CombatSystem::GetInstance()
void luaopen_combat_system(lua_State* L);