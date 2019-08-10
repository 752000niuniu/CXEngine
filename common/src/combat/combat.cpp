#include "combat.h"

TurnCommand::TurnCommand()
{

}


TurnCommand::~TurnCommand()
{

}

#ifndef SIMPLE_SERVER
#include "message.h"
#include "animation/frame_animation.h"
#include "skill.h"
#include "actor/player_state.h"
#include "graphics/renderer.h"
#include "input_manager.h"

static	FrameAnimation* s_CombatBG;
CombatSystem::CombatSystem()
	:m_Actors(0),
	m_TurnCounter(0),
	m_BattleState(BATTLE_DEFAULT)
{
	
	
	//SKILL_MANAGER_INSTANCE;
	//auto f = [](int role_id, float x, float y, std::wstring nickname)
	//{
	//	Player* p = new Player(role_id);
	//	p->SetActionID(Action::Idle);
	//	p->SetPos(x, y);
	//	p->SetCombatPos(x, y);
	//	p->SetCombatTargetPos({ x*1.0f,y*1.0f });
	//	p->SetIsCombat(true);
	//	//p->SetNickName(nickname);
	//	p->ChangeWeapon(0);
	//	return p;
	//};
	//AddSelf(0, f(3, 415.0f / 640 * WINDOW_INSTANCE->GetWidth(), 275.0f / 480 * WINDOW_INSTANCE->GetHeight(), L"\u5df1\u65b9\u7ec40")); //己方组
	//AddSelf(1, f(3, 355.0f / 640 * WINDOW_INSTANCE->GetWidth(), 305.0f / 480 * WINDOW_INSTANCE->GetHeight(), L"\u5df1\u65b9\u7ec401"));
	//AddSelf(2, f(3, 475.0f / 640 * WINDOW_INSTANCE->GetWidth(), 245.0f / 480 * WINDOW_INSTANCE->GetHeight(), L"\u5df1\u65b9\u7ec402"));
	//AddSelf(3, f(3, 295.0f / 640 * WINDOW_INSTANCE->GetWidth(), 335.0f / 480 * WINDOW_INSTANCE->GetHeight(), L"\u5df1\u65b9\u7ec403"));
	//AddSelf(4, f(3, 535.0f / 640 * WINDOW_INSTANCE->GetWidth(), 215.0f / 480 * WINDOW_INSTANCE->GetHeight(), L"\u5df1\u65b9\u7ec404"));
	//AddSelf(5, f(3, 465.0f / 640 * WINDOW_INSTANCE->GetWidth(), 315.0f / 480 * WINDOW_INSTANCE->GetHeight(), L"\u5df1\u65b9\u7ec405"));
	//AddSelf(6, f(3, 405.0f / 640 * WINDOW_INSTANCE->GetWidth(), 345.0f / 480 * WINDOW_INSTANCE->GetHeight(), L"\u5df1\u65b9\u7ec406"));
	//AddSelf(7, f(3, 525.0f / 640 * WINDOW_INSTANCE->GetWidth(), 285.0f / 480 * WINDOW_INSTANCE->GetHeight(), L"\u5df1\u65b9\u7ec407"));
	//AddSelf(8, f(3, 345.0f / 640 * WINDOW_INSTANCE->GetWidth(), 375.0f / 480 * WINDOW_INSTANCE->GetHeight(), L"\u5df1\u65b9\u7ec408"));
	//AddSelf(9, f(3, 585.0f / 640 * WINDOW_INSTANCE->GetWidth(), 255.0f / 480 * WINDOW_INSTANCE->GetHeight(), L"\u5df1\u65b9\u7ec409"));

	//AddEnemy(0, f(4, 175.0f / 640 * WINDOW_INSTANCE->GetWidth(), 170.0f / 480 * WINDOW_INSTANCE->GetHeight(), L"\u654c\u65b9\u7ec40"));	//敌方组
	//AddEnemy(1, f(4, 115.0f / 640 * WINDOW_INSTANCE->GetWidth(), 200.0f / 480 * WINDOW_INSTANCE->GetHeight(), L"\u654c\u65b9\u7ec401"));
	//AddEnemy(2, f(4, 235.0f / 640 * WINDOW_INSTANCE->GetWidth(), 140.0f / 480 * WINDOW_INSTANCE->GetHeight(), L"\u654c\u65b9\u7ec402"));
	//AddEnemy(3, f(4, 55.0f / 640 * WINDOW_INSTANCE->GetWidth(), 230.0f / 480 * WINDOW_INSTANCE->GetHeight(), L"\u654c\u65b9\u7ec403"));
	//AddEnemy(4, f(4, 295.0f / 640 * WINDOW_INSTANCE->GetWidth(), 110.0f / 480 * WINDOW_INSTANCE->GetHeight(), L"\u654c\u65b9\u7ec404"));
	//AddEnemy(5, f(4, 220.0f / 640 * WINDOW_INSTANCE->GetWidth(), 210.0f / 480 * WINDOW_INSTANCE->GetHeight(), L"\u654c\u65b9\u7ec405"));
	//AddEnemy(6, f(4, 160.0f / 640 * WINDOW_INSTANCE->GetWidth(), 240.0f / 480 * WINDOW_INSTANCE->GetHeight(), L"\u654c\u65b9\u7ec406"));
	//AddEnemy(7, f(4, 280.0f / 640 * WINDOW_INSTANCE->GetWidth(), 180.0f / 480 * WINDOW_INSTANCE->GetHeight(), L"\u654c\u65b9\u7ec407"));
	//AddEnemy(8, f(4, 100.0f / 640 * WINDOW_INSTANCE->GetWidth(), 270.0f / 480 * WINDOW_INSTANCE->GetHeight(), L"\u654c\u65b9\u7ec408"));
	//AddEnemy(9, f(4, 340.0f / 640 * WINDOW_INSTANCE->GetWidth(), 150.0f / 480 * WINDOW_INSTANCE->GetHeight(), L"\u654c\u65b9\u7ec409"));

	//Sprite* sp = ResourceManager::GetInstance()->LoadWASSprite(AddonWDF, 0x708C11A0);
	//while (!sp)
	//{
	//	sp = ResourceManager::GetInstance()->LoadWASSprite(AddonWDF, 0x708C11A0);

	//}
	//TextureManager::GetInstance()->LoadTexture(sp->mPath, sp->mWidth, sp->mHeight, true, (uint8_t*)sp->mFrames[0].src.data());
	//m_CombatBGPath = sp->mPath;

	// RENDERER_2D_INSTANCE->AddObject(new Image(
	// 	s_CombatBG->GetFramePath(0), Vec2(0, 0), Vec2(WINDOW_INSTANCE->GetWidth(), WINDOW_INSTANCE->GetHeight()))
	// );
}

CombatSystem::~CombatSystem()
{

}

//void CombatSystem::AddEnemy(int pos, Player* enemy)
//{
//	int dir = static_cast<int>(Direction::S_E);
//	enemy->ResetDirAll(dir);
//	enemy->SetActorID(10 + pos);
//	GAME_ENTITY_MANAGER_INSTANCE->RegisterEntity(enemy);
//	m_Enemies.push_back(enemy);
//}
//
//void CombatSystem::AddSelf(int pos, Player* self)
//{
//	int dir = static_cast<int>(Direction::N_W);
//	self->ResetDirAll(dir);
//	self->SetActorID(pos);
//	GAME_ENTITY_MANAGER_INSTANCE->RegisterEntity(self);
//	m_Ourselves.push_back(self);
//}

void CombatSystem::Update()
{
	if (m_BattleState == BATTLE_DEFAULT) {

	}
	else if (m_BattleState == BATTLE_START) {
		m_TurnCounter = 0;
		for (auto& it : m_Actors) {
			it->SetIsCombat(true);
			it->SetActionID(Action::Batidle);
		}
		m_BattleState = BTTALE_TURN_NEXT;
	}
	else if (m_BattleState == BATTLE_TURN_STAND_BY) {
		bool allReady = true;
		for (auto& it : m_Actors) {
			if (!it->IsTurnReady()) {
				allReady = false;
				break;
			}
		}
		if (allReady) {
			m_BattleState = BTTALE_TURN_EXECUTE;
		}
	}
	else if (m_BattleState == BTTALE_TURN_EXECUTE) {
		if (m_Commands.size() > 0) {
			TurnCommand& cmd = m_Commands.front();
			if (cmd.Finished()) {
				m_Commands.pop_front();
			}
			else {
				cmd.Update();
			}
		}
		if (m_Commands.size() == 0) {
			m_BattleState = BTTALE_TURN_NEXT;
		}
	}
	else if (m_BattleState == BTTALE_TURN_NEXT) {
		m_TurnCounter++;
		m_BattleState = BATTLE_TURN_STAND_BY;
		for (auto& it : m_Actors) {
			it->SetTurnReady(false);
		}
	}
	else if (m_BattleState == BATTLE_END) {
		m_TurnCounter = 0;
		for (auto& it : m_Actors) {
			it->SetIsCombat(false);
		}
	}
	
	if (m_BattleState != BATTLE_DEFAULT && m_BattleState != BATTLE_DEFAULT) {
		for (auto& it : m_Actors) {
			it->OnUpdate();
		}
	}

	//INPUT_MANAGER_INSTANCE->RegisterOnKeyClickEvent(GLFW_KEY_1,
	//	[this]() {
	//	int selfID = RANDOM_INSTANCE->NextInt(0, (int)m_Ourselves.size() - 1);
	//	int enemyID = RANDOM_INSTANCE->NextInt(0, (int)m_Enemies.size() - 1);
	//	auto& self = m_Ourselves[selfID];
	//	auto& enemy = m_Enemies[enemyID];
	//	self->SetCombatTargetPos({ enemy->GetCombatPos().x + 88 , enemy->GetCombatPos().y + 73 });
	//	self->SetTargetID(enemyID + 10);
	//	self->GetFSM()->ChangeState(PlayerCombatMoveState::GetInstance());
	//}
	//);

	//INPUT_MANAGER_INSTANCE->RegisterOnKeyClickEvent(GLFW_KEY_2,
	//	[this]() {
	//	int selfID = RANDOM_INSTANCE->NextInt(0, (int)m_Enemies.size() - 1);
	//	auto& self = m_Ourselves[selfID];
	//	self->GetFSM()->ChangeState(PlayerCombatCastAttackState::GetInstance());
	//}
	//);


	//float dt = WINDOW_INSTANCE->GetDeltaTime();
	//for (auto* self : m_Ourselves)
	//{
	//	if (self != nullptr)
	//	{
	//		self->OnUpdate(dt);
	//	}
	//}

	//for (auto* enemy : m_Enemies)
	//{
	//	if (enemy != nullptr)
	//	{
	//		enemy->OnUpdate(dt);
	//	}
	//}

}

void CombatSystem::Draw()
{
	if (m_BattleState != BATTLE_DEFAULT && m_BattleState != BATTLE_DEFAULT) {
		for (auto& it : m_Actors) {
			it->OnDraw();
		}
	}
	
	//auto* texture = TextureManager::GetInstance()->GetTexture(m_CombatBGPath);
	//SPRITE_RENDERER_INSTANCE->DrawFrameSprite(texture,
	//	glm::vec2(0, 0),
	//	glm::vec2(WINDOW_INSTANCE->GetWidth(), WINDOW_INSTANCE->GetWidth()), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));

	////RENDERER_2D_INSTANCE->Render();

	//for (auto* enemy : m_Enemies)
	//{
	//	if (enemy != nullptr)
	//	{
	//		Pos pos = enemy->GetCombatPos();
	//		enemy->OnDraw(static_cast<int>(pos.x), static_cast<int>(pos.y));
	//	}
	//}

	//for (auto* self : m_Ourselves)
	//{
	//	if (self != nullptr)
	//	{
	//		Pos pos = self->GetCombatPos();
	//		self->OnDraw(static_cast<int>(pos.x), static_cast<int>(pos.y));
	//	}
	//}
	////m_OtherPtr->OnDraw(m_RendererPtr, m_OtherPtr->GetX() + mapOffsetX,m_OtherPtr->GetY() + mapOffsetY);
}




#else
CombatSystem::CombatSystem()
{
}

CombatSystem::~CombatSystem()
{

}



void CombatSystem::Update()
{
	
}

void CombatSystem::Draw()
{
}

#endif // !SIMPLE_SERVER

void CombatSystem::NextTurn()
{
	m_TurnCounter++;
}

void CombatSystem::StartBattle()
{
	m_BattleState = BATTLE_START;
}

void CombatSystem::EndBattle()
{
	m_BattleState = BATTLE_END;
}
