#include "actor.h"
#include "scene/game_map.h"
#include "actor/player.h"
#include "scene/scene_manager.h"
#include "utils.h"
#include "lua.hpp"
#include "window.h"
#include "cxmath.h"
#include "actor_manager.h"
#ifndef SIMPLE_SERVER
#include "action.h"
#include "input_manager.h"
#include "animation/sprite.h"
#include "text_renderer.h"
#include "resource_manager.h"
#endif
#include "move.h"


#define ACTOR_METATABLE_NAME "mt_actor"

Actor::Actor(uint64_t pid)
	:BaseGameEntity(pid),
	m_RoleID(0),
	m_NickName(""),
	m_WeaponID(-1),
	m_ActionID(ACTION_IDLE),
	m_Pos(0, 0),
	m_MoveToPos(0, 0),
	m_Dir(Direction::S),
	m_IsMove(false),
	m_MoveVelocity(150),
	m_bInCombat(false),
	m_TargetID(-1),
	m_bSkillFrameShow(false),
	m_IsLocalPlayer(false),
	m_IsAutoRun(false),
	m_FrameSpeed(0.080f),
	m_DirCount(8),
	m_ActorType(ACTOR_TYPE_PLAYER),
	m_AvatarID(""),
	m_WeaponAvatarID("")
{
	
	m_MoveHandle = new MoveHandle(this);
#ifndef SIMPLE_SERVER
	m_SayWidget = new TextView();
	m_SayWidget->PaddingHorizontal = 4;
	m_SayWidget->PaddingVertical = 2;
	m_SayWidget->ShowEmotion = true;
	m_SayWidget->BackgroundResID = RESOURCE_MANAGER_INSTANCE->EncodeWAS(WZIFEWDF, 0xEF073E43); //F3FAAAF2´ó¿ò
																							   //m_SayWidget->BackgroundResID = RESOURCE_MANAGER_INSTANCE->EncodeWAS(WZIFEWDF, 0xA4120EA9);
	m_SayDuration = 0;
	m_ASM = new ActionStateMachine(this);  
	INPUT_MANAGER_INSTANCE->RegisterView(this);
	PathMoveAction*action = new PathMoveAction(this);
	m_ASM->ChangeAction(action);
#endif

	
	
}
Actor::~Actor()
{
	SafeDelete(m_MoveHandle);
#ifndef SIMPLE_SERVER
	INPUT_MANAGER_INSTANCE->UnRegisterView(this);
	SafeDelete(m_ASM);
	SafeDelete(m_SayWidget);
#endif
}

void Actor::OnUpdate()
{
	m_MoveHandle->Update();
#ifndef SIMPLE_SERVER
	m_ASM->Update();
	if (m_SayDuration > 0)
	{
		if (m_SayWidget)
		{
			m_SayWidget->OnUpdate();
		}
	}
#endif
}

void Actor::OnDraw()
{
#ifndef SIMPLE_SERVER
	m_ASM->Draw();
	if (!m_Name.empty())
	{
		auto* avatar = m_ASM->GetAvatar();
		auto green = glm::vec3(115 / 255.0f, 1.0f, 137 / 255.0f);
		TextRenderer::GetInstance()->DrawTextC(m_Name.c_str(),
			((int)avatar->Pos.x),
			((int)avatar->Pos.y + 20),
			TextRenderer::CENTER
		);
	}

	if (m_SayDuration > 0)
	{
		int past = (int)WINDOW_INSTANCE->GetDeltaTimeMilliseconds();
		m_SayDuration -= past;

		if (m_SayWidget->Background != nullptr)
		{
			int bgWidth = m_SayWidget->Width;
			int bgHeight = m_SayWidget->Height;
			auto* avatar = m_ASM->GetAvatar();
			m_SayWidget->X = (int)avatar->Pos.x - bgWidth / 2;
			m_SayWidget->Y = (int)avatar->Pos.y - avatar->KeyY - bgHeight;
		}
		m_SayWidget->OnDraw();
	}
#endif
}

void Actor::SetDir(int dir)
{
	m_Dir = dir;
#ifndef SIMPLE_SERVER
	auto* avatar = m_ASM->GetAvatar();
	if(avatar){
		if (avatar->GroupCount == 4) {
			m_Dir = GMath::Dir8toDir4(m_Dir);
		}
	}
#endif
}

void Actor::ReverseDir()
{
	SetDir(GMath::GetReverseDir(m_Dir));
}

void Actor::SetActionID(int action)
{
	if (m_ActorType == ACTOR_TYPE_PLAYER) {
		m_ActionID = action;
	}
	else {
		if (action_is_emotion_action(action))return;
		m_ActionID = action;
	}
}

void Actor::SetRoleID(int id)
{
	m_RoleID = id;
}

void Actor::SetWeaponID(int weapon)
{
	m_WeaponID = weapon;
}

void Actor::SetPos(float x, float y)
{
	if (IsCombat()) {
		m_CombatProps.Pos.x = x;
		m_CombatProps.Pos.y = y;
	}
	else {
		m_Pos.x = x; m_Pos.y = y;
	}
}

void Actor::SetPos(Pos p)
{
	if (IsCombat()) {
		m_CombatProps.Pos = p;
	}
	else {
		m_Pos = p;
	}
}

void Actor::SetMoveToPos(Pos dest)
{
	m_MoveToPos.x = dest.x;
	m_MoveToPos.y = dest.y;
}

float Actor::GetWidth()
{
#ifndef SIMPLE_SERVER
	auto* avatar = m_ASM->GetAvatar();
	if (!avatar)return 0;
	return (float)avatar->Width;
#else
	return 0;
#endif
}

float Actor::GetHeight()
{
#ifndef SIMPLE_SERVER
	auto* avatar = m_ASM->GetAvatar();
	if (!avatar)return 0;
	return (float)avatar->Height;
#else
	return 0;
#endif
}

int Actor::GetDirByDegree(float degree)
{
#ifndef SIMPLE_SERVER
	int dircnt = m_ASM->GetDirCount();
	if (dircnt == 8) {
		return GMath::Astar_GetDir(degree);
	}
	else {
		return GMath::Astar_GetDir4(degree);
	}
#else
	return GMath::Astar_GetDir(degree);
#endif
}

bool Actor::IsLocal()
{
	return actor_manager_is_local_player(this);
}
float Actor::GetCombatDistSquare()
{
	Pos& pos = GetPos();
	Pos& targetPos = GetCombatTargetPos();
	return ::GMath::Astar_GetDistanceSquare(m_CombatProps.Pos.x, m_CombatProps.Pos.y, m_CombatProps.TargetPos.x, m_CombatProps.TargetPos.y);
}

float Actor::GetMoveDestDistSquare(Pos dest)
{
	return ::GMath::Astar_GetDistanceSquare(m_Pos.x, m_Pos.y, dest.x, dest.y);
}
float Actor::GetCombatAngle()
{
	Pos& pos = GetPos();
	Pos& targetPos = GetCombatTargetPos();
	return ::GMath::Astar_GetAngle(m_CombatProps.Pos.x, m_CombatProps.Pos.y, m_CombatProps.TargetPos.x, m_CombatProps.TargetPos.y);
}

float Actor::GetMoveDestAngle(Pos dest)
{
	return ::GMath::Astar_GetAngle(m_Pos.x, m_Pos.y, dest.x, dest.y);
}

BaseScene* Actor::GetScene()
{
	return scene_manager_fetch_scene(m_SceneID);
}

CXString Actor::GetWeaponAvatarID()
{
	if (m_ActorType == ACTOR_TYPE_NPC)
		return "";
	return m_WeaponAvatarID;
}
#ifndef SIMPLE_SERVER
Bound Actor::GetViewBounds()
{
	auto* avatar = m_ASM->GetAvatar();
	if (avatar == nullptr) return { 0,0,0,0 };
	Pos pos = avatar->Pos;
	pos.x -= avatar->KeyX;
	pos.y -= avatar->KeyY;
	return Bound{ pos.x, (pos.x + avatar->Width),
		pos.y,(pos.y + avatar->Height) };
}

bool Actor::CheckDrag(int dx, int dy)
{
	return pow(dx, 2) + pow(dy, 2) >= 16;
}

void Actor::OnDragMove(int dx, int dy)
{
	m_Pos.x += (float)dx;
	m_Pos.y += (float)dy;
}

void Actor::Say(std::string Text)
{
	std::wstring wText = utils::StringToWstring(Text);
	m_SayDuration = 1000 * 60 * 24;
	m_SayWidget->Text = wText;
	m_SayWidget->TextCache = std::vector<uint32_t>(wText.begin(), wText.end());
}

#endif

Actor* lua_check_actor(lua_State*L, int index)
{
	Actor** ptr = (Actor**)lua_touserdata(L, index);
	return *ptr;
}

int actor_destroy(lua_State * L)
{
	Actor** ptr = (Actor**)lua_touserdata(L, 1);
	Actor* actor = *ptr;
	if (actor) {
		SafeDelete(actor);
	}
	return 0;
}
int actor_update(lua_State * L) {
	Actor* actor = lua_check_actor(L, 1);
	actor->OnUpdate();
	return 0;
}

int actor_draw(lua_State * L) {
	Actor* actor = lua_check_actor(L, 1);
	//Pos pos = actor->GetPos();
	actor->OnDraw();
	return 0;
}
int actor_set_pos(lua_State* L)
{
	Actor* actor = lua_check_actor(L, 1);
	lua_Number x = lua_tonumber(L, 2);
	lua_Number y = lua_tonumber(L, 3);
	actor->SetPos({ (float)x,(float)y });
	return 0;
}

int actor_set_dir(lua_State* L)
{
	Actor* actor = lua_check_actor(L, 1);
	lua_Number dir = lua_tonumber(L, 2);
	actor->SetDir((int)dir);
	return 0;
}
int actor_get_dir(lua_State* L) {
	Actor* actor = lua_check_actor(L, 1);
	lua_pushnumber(L, actor->GetDir());
	return 1;
}

int actor_reverse_dir(lua_State* L) {
	Actor* actor = lua_check_actor(L, 1);
	actor->ReverseDir();
	return 0;
}

int actor_set_type(lua_State* L)
{
	Actor* actor = lua_check_actor(L, 1);
	int type = (int)lua_tointeger(L, 2);
	actor->SetType(type);
	return 0;
}
int actor_get_type(lua_State* L) {
	Actor* actor = lua_check_actor(L, 1);
	lua_pushinteger(L, actor->GetType());
	return 1;
}


int actor_set_local(lua_State* L)
{
	Actor* actor = lua_check_actor(L, 1);
	bool  local = (bool)lua_toboolean(L, 2);
	actor->SetLocal(local);
	return 0;
}

int actor_is_local(lua_State* L) {
	Actor* actor = lua_check_actor(L, 1);
	lua_pushboolean(L, actor_manager_is_local_player(actor));
	return 1;
}

int actor_set_is_combat(lua_State* L)
{
	Actor* actor = lua_check_actor(L, 1);
	bool  is_combat = (bool)lua_toboolean(L, 2);
	actor->SetIsCombat(is_combat);
	return 0;
}

int actor_is_combat(lua_State* L) {
	Actor* actor = lua_check_actor(L, 1);
	lua_pushboolean(L, actor->IsCombat());
	return 1;
}

int actor_get_id(lua_State* L) {
	Actor* actor = lua_check_actor(L, 1);
	lua_pushinteger(L, actor->GetID());
	return 1;
}

int actor_set_action_id(lua_State* L) {
	Actor* actor = lua_check_actor(L, 1);
	int actionID = (int)lua_tointeger(L, 2);
	actor->SetActionID(actionID);
#ifndef SIMPLE_SERVER
	Action* action = new Action(actor);
	actor->GetASM()->ChangeAction(action);
#endif
	return 0;
}

int actor_get_action_id(lua_State* L) {
	Actor* actor = lua_check_actor(L, 1);
	lua_pushinteger(L, actor->GetActionID());
	return 1;
}

int actor_translate_x(lua_State* L) {
	Actor* actor = lua_check_actor(L, 1);
	float x = (float)luaL_optnumber(L, 2, 0.f);
	actor->TranslateX(x);
	return 0;
}

int actor_translate_y(lua_State* L) {
	Actor* actor = lua_check_actor(L, 1);
	float y = (float)luaL_optnumber(L, 2, 0.f);
	actor->TranslateY(y);
	return 0;
}


int actor_move_to(lua_State* L){
	Actor* actor = lua_check_actor(L, 1);
	float x = (float)lua_tonumber(L, 2);
	float y = (float)lua_tonumber(L, 3);
	actor->GetMoveHandle()->MoveTo(x, y);
	
	return 0;
}
int actor_say(lua_State* L) {
#ifndef SIMPLE_SERVER
	Actor* actor = lua_check_actor(L, 1);
	const char* msg = lua_tostring(L, 2);
	actor->Say(msg);
#endif
	return 0;
}

int actor_set_x(lua_State* L) {
	Actor* actor = lua_check_actor(L, 1);
	float x = (float)lua_tonumber(L, 2);
	actor->SetX(x);
	return 0;
}
int actor_get_x(lua_State* L) {
	Actor* actor = lua_check_actor(L, 1);
	lua_pushnumber(L, actor->GetX());
	return 1;
}

int actor_set_y(lua_State* L) {
	Actor* actor = lua_check_actor(L, 1);
	float y = (float)lua_tonumber(L, 2);
	actor->SetY(y);
	return 0;
}

int actor_get_y(lua_State* L) {
	Actor* actor = lua_check_actor(L, 1);
	lua_pushnumber(L, actor->GetY());
	return 1;
}


int actor_get_pos(lua_State* L) {
	Actor* actor = lua_check_actor(L, 1);
	lua_pushnumber(L, actor->GetX());
	lua_pushnumber(L, actor->GetY());
	return 2;
}


int actor_get_width(lua_State* L) {
	Actor* actor = lua_check_actor(L, 1);
	lua_pushnumber(L, actor->GetWidth());
	return 1;
}
int actor_get_height(lua_State* L) {
	Actor* actor = lua_check_actor(L, 1);
	lua_pushnumber(L, actor->GetHeight());
	return 1;
}



int actor_set_scene_id(lua_State* L) {
	Actor* actor = lua_check_actor(L, 1);
	int sceneID = (int)lua_tointeger(L, 2);
	actor->SetSceneID(sceneID);
	return 0;
}

int actor_get_scene_id(lua_State* L) {
	Actor* actor = lua_check_actor(L, 1);
	lua_pushinteger(L, actor->GetSceneID());
	return 1;
}
int actor_set_name(lua_State* L) {
	Actor* actor = lua_check_actor(L, 1);
	const char* name = lua_tostring(L, 2);
	actor->SetName(name);
	return 0;
}
int actor_get_name(lua_State* L) {
	Actor* actor = lua_check_actor(L, 1);
	auto name = actor->GetName();
	lua_pushstring(L, name.c_str());
	return 1;
}

int actor_set_role_id(lua_State* L) {
	Actor* actor = lua_check_actor(L, 1);
	int roleID = (int)lua_tointeger(L, 2);
	actor->SetRoleID(roleID);
#ifndef SIMPLE_SERVER 
	actor->GetASM()->Reset();
#endif // !SIMPLE_SERVER
	return 0;
}
int actor_get_role_id(lua_State* L) {
	Actor* actor = lua_check_actor(L, 1);
	lua_pushinteger(L, actor->GetRoleID());
	return 1;
}
int actor_set_weapon_id(lua_State* L) {
	Actor* actor = lua_check_actor(L, 1);
	int weaponID = (int)lua_tointeger(L, 2);
	actor->SetWeaponID(weaponID);
#ifndef SIMPLE_SERVER
	actor->GetASM()->Reset();
#endif // !SIMPLE_SERVER
	return 0;
}

int actor_get_weapon_id(lua_State* L) {
	Actor* actor = lua_check_actor(L, 1);
	lua_pushinteger(L, actor->GetWeaponID());
	return 1;
}

int actor_set_avatar_id(lua_State* L) {
	Actor* actor = lua_check_actor(L, 1);
	auto id = lua_tostring(L, 2);
	actor->SetAvatarID(id);
#ifndef SIMPLE_SERVER
	actor->GetASM()->Reset();
#endif // !SIMPLE_SERVER
	return 0;
}

int actor_get_avatar_id(lua_State* L) {
	Actor* actor = lua_check_actor(L, 1);
	lua_pushstring(L, actor->GetAvatarID().c_str());
	return 1;
}

int actor_set_weapon_avatar_id(lua_State* L) {
	Actor* actor = lua_check_actor(L, 1);
	auto id = lua_tostring(L, 2);
	actor->SetWeaponAvatarID(id);
#ifndef SIMPLE_SERVER
	actor->GetASM()->Reset();
#endif // !SIMPLE_SERVER
	return 0;
}

int actor_get_weapon_avatar_id(lua_State* L) {
	Actor* actor = lua_check_actor(L, 1);
	lua_pushstring(L, actor->GetWeaponAvatarID().c_str());
	return 1;
}

int actor_clear_frames(lua_State* L) {

	return 0;
}

int actor_play_attack(lua_State*L){
#ifndef SIMPLE_SERVER 
	Actor* actor = lua_check_actor(L, 1);
	Actor* target = lua_check_actor(L, 2);
	AttackAction* action = new AttackAction(actor);
	action->AddTarget(target);
	actor->GetASM()->ChangeAction(action);
#endif
	return 0;
}

int actor_play_cast(lua_State*L) {
#ifndef SIMPLE_SERVER 
	Actor* actor = lua_check_actor(L, 1);
	Actor* target = lua_check_actor(L, 2);
	uint64_t skill = (uint64_t)lua_tointeger(L, 3);
	actor->SetCastID(skill);
	CastAction* action = new CastAction(actor, target, skill);
	actor->GetASM()->ChangeAction(action);
#endif
	return 0;
}

int actor_set_time_interval(lua_State*L) {
#ifndef SIMPLE_SERVER
	Actor* actor = lua_check_actor(L, 1);
	float ti = (float)lua_tonumber(L, 2);
	actor->GetASM()->SetTimeInterval(ti);
#endif
	return 0;
}

int actor_get_avatar(lua_State*L) {
#ifndef SIMPLE_SERVER
	Actor* actor = lua_check_actor(L, 1);
	auto* avatar = actor->GetASM()->GetAvatar();
	if (avatar == nullptr)return 0;
	lua_push_animation(L, avatar);
	return 1;
#else
	return 0;
#endif
}

int actor_get_weapon(lua_State*L) {
#ifndef SIMPLE_SERVER
	Actor* actor = lua_check_actor(L, 1);
	auto* weapon = actor->GetASM()->GetWeapon();
	if (weapon == nullptr)return 0;
	lua_push_animation(L, weapon);
	return 1;
#else
	return 0;
#endif
}

luaL_Reg mt_actor[] = {
//{ "__gc",actor_destroy },
{ "Destroy",actor_destroy },
{ "Update",actor_update},
{ "Draw",actor_draw },
{ "SetType", actor_set_type },
{ "GetType", actor_get_type },
{ "SetPos",actor_set_pos},
{"SetX", actor_set_x},
{"GetX", actor_get_x},
{"SetY", actor_set_y},
{"GetY", actor_get_y},
{ "GetPos", actor_get_pos },
{ "GetWidth", actor_get_width},
{ "GetHeight", actor_get_height },
{ "SetIsCombat", actor_set_is_combat},
{ "IsCombat", actor_is_combat },
{"GetID", actor_get_id},
{"SetSceneID", actor_set_scene_id},
{"GetSceneID", actor_get_scene_id},
{"SetName", actor_set_name},
{"GetName", actor_get_name},
{"SetRoleID", actor_set_role_id},
{"GetRoleID", actor_get_role_id},
{"SetWeaponID", actor_set_weapon_id},
{"GetWeaponID", actor_get_weapon_id},
{"SetWeaponAvatarID", actor_set_weapon_avatar_id},
{"GetWeaponAvatarID", actor_get_weapon_avatar_id},
{"SetAvatarID", actor_set_avatar_id},
{"GetAvatarID", actor_get_avatar_id},
{"SetDir", actor_set_dir},
{"GetDir", actor_get_dir},
{ "ReverseDir", actor_reverse_dir},
{"SetLocal", actor_set_local},
{"IsLocal", actor_is_local},
{"SetActionID", actor_set_action_id},
{"GetActionID", actor_get_action_id},
{"TranslateX", actor_translate_x},
{ "TranslateY", actor_translate_y },
{"MoveTo", actor_move_to},
{"Say", actor_say},
{"ClearFrames", actor_clear_frames},
{ "PlayAttack", actor_play_attack},
{ "PlayCast", actor_play_cast},
{ "SetTimeInterval", actor_set_time_interval },
{ "GetAvatar", actor_get_avatar},
{ "GetWeapon", actor_get_weapon},
{ NULL, NULL }
};

void lua_push_actor(lua_State*L, Actor* actor)
{
	Actor** ptr = (Actor**)lua_newuserdata(L, sizeof(Actor*));
	*ptr = actor;
	if (luaL_newmetatable(L, ACTOR_METATABLE_NAME)) {
		luaL_setfuncs(L, mt_actor, 0);
		lua_pushvalue(L, -1);
		lua_setfield(L, -2, "__index");
	}
	lua_setmetatable(L, -2);
}



int lua_new_actor(lua_State* L)
{
	int type = (int)luaL_optinteger(L, 1, ACTOR_TYPE_DEFAULT);
	int rold_id = (int)luaL_optinteger(L, 2, 0);
	
	Actor* actor = new Player(rold_id);
	actor->SetType(type);
	
	lua_push_actor(L, actor);
	return 1;
}

int actor_get_metatable(lua_State* L){
	if (luaL_newmetatable(L, ACTOR_METATABLE_NAME)) {
		luaL_setfuncs(L, mt_actor, 0);
		lua_pushvalue(L, -1);
		lua_setfield(L, -2, "__index");
	}
	return 1;
}

int action_system_get_action_size() {
	return (int)action_get_size();
}

std::string action_system_get_action_name(int action) {
	return action_get_name(action);
}

void luaopen_actor(lua_State* L)
{
	script_system_register_luac_function(L, lua_new_actor);
	script_system_register_luac_function(L, actor_get_metatable);

	script_system_register_function(L, action_system_get_action_size);
	script_system_register_function(L, action_system_get_action_name);


#define REG_ENUM(e) (lua_pushinteger(L, e), lua_setglobal(L, #e))
	REG_ENUM(ACTOR_TYPE_DEFAULT);
	REG_ENUM(ACTOR_TYPE_PLAYER);
	REG_ENUM(ACTOR_TYPE_PET);
	REG_ENUM(ACTOR_TYPE_NPC);

	REG_ENUM(ACTION_IDLE);
	REG_ENUM(ACTION_WALK);
	REG_ENUM(ACTION_SIT);
	REG_ENUM(ACTION_ANGRY);
	REG_ENUM(ACTION_SAYHI);
	REG_ENUM(ACTION_DANCE);
	REG_ENUM(ACTION_SALUTE);
	REG_ENUM(ACTION_CLPS);
	REG_ENUM(ACTION_CRY);
	REG_ENUM(ACTION_BATIDLE);
	REG_ENUM(ACTION_ATTACK);
	REG_ENUM(ACTION_CAST);
	REG_ENUM(ACTION_BEHIT);
	REG_ENUM(ACTION_RUNTO);
	REG_ENUM(ACTION_RUNBACK);
	REG_ENUM(ACTION_DEFEND);


	REG_ENUM(WEAPON_SPEAR);
	REG_ENUM(WEAPON_AXE);
	REG_ENUM(WEAPON_SWORD);
	REG_ENUM(WEAPON_DBSWORDS);
	REG_ENUM(WEAPON_RIBBON);
	REG_ENUM(WEAPON_CLAW);
	REG_ENUM(WEAPON_FAN);
	REG_ENUM(WEAPON_WAND);
	REG_ENUM(WEAPON_HAMMER);
	REG_ENUM(WEAPON_WHIP);
	REG_ENUM(WEAPON_RING);
	REG_ENUM(WEAPON_KNIFE);
	REG_ENUM(WEAPON_STAFF);
	REG_ENUM(WEAPON_JEWEL);
	REG_ENUM(WEAPON_BOW);


	REG_ENUM(ROLE_FYN);
	REG_ENUM(ROLE_YNX);
	REG_ENUM(ROLE_XYS);
	REG_ENUM(ROLE_JXK);
	REG_ENUM(ROLE_HMR);
	REG_ENUM(ROLE_GJL);
	REG_ENUM(ROLE_JMW);
	REG_ENUM(ROLE_HTG);
	REG_ENUM(ROLE_WTJ);
	REG_ENUM(ROLE_XCE);
	REG_ENUM(ROLE_STB);
	REG_ENUM(ROLE_LTZ);

	REG_ENUM(DIR_N);
	REG_ENUM(DIR_S);
	REG_ENUM(DIR_W);
	REG_ENUM(DIR_E);
	REG_ENUM(DIR_NE);
	REG_ENUM(DIR_NW);
	REG_ENUM(DIR_SE);
	REG_ENUM(DIR_SW);
#undef REG_ENUM

}
