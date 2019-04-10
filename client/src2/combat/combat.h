#pragma once


#include "define_types.h"
#include "../actor/Player.h"
#include "singleton.h"


class CombatSystem : public Singleton<CombatSystem>
{
public:
	CombatSystem();
	~CombatSystem();
	
	void AddEnemy(int pos,Player* enemy);
	void AddSelf(int pos,Player* self);
	void ProcessInput();
	void Update();
	void Draw();
private:
	std::vector<Player*> m_Ourselves;
	std::vector<Player*> m_Enemies;
	std::string m_CombatBGPath;
};


#define COMBAT_SYSTEM_INSTANCE CombatSystem::GetInstance()
