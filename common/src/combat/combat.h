#pragma once


#include "define_types.h"
#include "actor/player.h"
#include "singleton.h"


/*
CombatSystem ս��������
1. ���Actors self/enemy
2. ս��ָ����У�����ս��ָ���ս��ָ������
3. �غ�ս��״̬�� ս����ʼ���غ�׼��������ȫ��ս��ָ����غϽ��У�����ս��ָ���ս������
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