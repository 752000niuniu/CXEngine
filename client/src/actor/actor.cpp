#include "actor.h"
#include "action.h"
#include "player.h"
#include "scene/scene_manager.h"
#include "utils.h"
#include "lua.hpp"
#include "window.h"

#define ACTOR_METATABLE_NAME "mt_actor"


Actor::Actor(int roleID):
m_RoleID(roleID),
m_NickName(""),
m_WeaponID(-1),
m_ActionID(Action::Idle),
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
m_FrameSpeed(0.080f)
{

}

Actor::~Actor()
{


}

Actor* lua_check_actor(lua_State*L, int index)
{
	Actor** ptr = (Actor**)lua_touserdata(L, index);
	return *ptr;
}

void player_set_frame_speed(int frame_speed)
{
	Player* player = SCENE_MANAGER_INSTANCE->GetCurrentScene()->GetLocalPlayer();
	if (player) {
		player->SetFrameSpeed((float)frame_speed);
	}
}

void player_set_move_speed(int move_speed)
{
	Player* player = SCENE_MANAGER_INSTANCE->GetCurrentScene()->GetLocalPlayer();
	if (player) {
		player->SetVelocity((float)move_speed);
	}
}

int actor_method_set_property(lua_State* L)
{
	Actor** ptr = (Actor**)lua_touserdata(L, 1);
	const char* prop_name = lua_tostring(L, 2);
	const char* prop_val = lua_tostring(L, 3);
	Actor* actor = *ptr;
	if (strcmp(prop_name, "name") == 0) {
		actor->SetNickName(prop_val);
	}
	return 0;
}

int actor_method_get_property(lua_State* L)
{
	Actor** ptr = (Actor**)lua_touserdata(L, 1);
	const char* prop_name = lua_tostring(L, 2);
	Actor* actor = *ptr;
	if (strcmp(prop_name, "name") == 0) {
		lua_pushstring(L, actor->GetNickName().c_str());
		return 1;
	}
	return 0;
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
	actor->OnUpdate(WINDOW_INSTANCE->GetDeltaTime());
	return 0;
}

int actor_draw(lua_State * L) {
	Actor* actor = lua_check_actor(L, 1);
	Pos pos = actor->GetPos();
	actor->OnDraw((int)pos.x ,(int) pos.y );
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

int actor_get_x(lua_State* L) {
	Actor* actor = lua_check_actor(L, 1);
	lua_pushnumber(L, actor->GetX());
	return 1;
}

int actor_get_y(lua_State* L) {
	Actor* actor = lua_check_actor(L, 1);
	lua_pushnumber(L, actor->GetY());
	return 1;
}

int actor_get_dir(lua_State* L) {
	Actor* actor = lua_check_actor(L, 1);
	lua_pushnumber(L, actor->GetDir());
	return 1;
}

int actor_set_action_id(lua_State* L) {
	Actor* actor = lua_check_actor(L, 1);
	int actionID = (int)lua_tointeger(L, 2);
	actor->SetActionID(actionID);
	return 0;
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

int actor_change_action(lua_State* L) {
	Actor* actor = lua_check_actor(L, 1);
	int action = (int)luaL_optinteger(L, 2, 0);
	if (actor->GetType() != ACTOR_TYPE_PLAYER) return 0;

	Player* player = dynamic_cast<Player*>(actor);
	player->ChangeAction(action);
	return 0;
}


int actor_change_weapon(lua_State* L) {
	Actor* actor = lua_check_actor(L, 1);
	int weapon= (int)luaL_optinteger(L, 2, 0);
	if (actor->GetType() != ACTOR_TYPE_PLAYER) return 0;

	Player* player = dynamic_cast<Player*>(actor);
	player->ChangeWeapon(weapon);
	return 0;

}


int actor_change_role(lua_State* L) {
	Actor* actor = lua_check_actor(L, 1);
	int role = (int)luaL_optinteger(L, 2, 0);
	if (actor->GetType() != ACTOR_TYPE_PLAYER) return 0;

	Player* player = dynamic_cast<Player*>(actor);
	player->ChangeRole(role);
	return 0;

}



luaL_Reg mt_actor[] = {
	{ "SetProperty",actor_method_set_property },
{ "GetProperty",actor_method_get_property },
{ "Destroy",actor_destroy },
{ "update",actor_update},
{ "draw",actor_draw },
{ "set_pos",actor_set_pos},
{ "set_dir",actor_set_dir },
{"GetX", actor_get_x},
{"GetY", actor_get_y},
{"GetDir", actor_get_dir},
{"SetActionID", actor_set_action_id},
{"TranslateX", actor_translate_x},
{ "TranslateY", actor_translate_y },

{ "ChangeAction", actor_change_action },
{ "ChangeWeapon", actor_change_weapon },
{ "ChangeRole", actor_change_role },

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
	
	Actor* actor = nullptr;
	if (type == ACTOR_TYPE_PLAYER) {
		actor = new Player(rold_id);
	}
	else if (type == ACTOR_TYPE_PET){
		actor = new Pet(rold_id);
	}
	else if (type == ACTOR_TYPE_NPC){
		actor = new Npc(rold_id);
	}
	
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
	script_system_register_function(L, player_set_frame_speed);
	script_system_register_function(L, player_set_move_speed);
	script_system_register_luac_function(L, lua_new_actor);
	script_system_register_luac_function(L, actor_get_metatable);

	script_system_register_function(L, action_system_get_action_size);
	script_system_register_function(L, action_system_get_action_name);

	std::map<std::string, int> action_enums;
	for (int i = Action::Idle; i <= Action::Defend; i++) {
		action_enums.insert({ action_get_name(i), i });
	}

	lua_rawgeti(L, LUA_REGISTRYINDEX, LUA_RIDX_GLOBALS);
#define REG_ENUM(e) lua_pushinteger(L, e);	lua_setfield(L, -2, #e);
	REG_ENUM(ACTOR_TYPE_DEFAULT);
	REG_ENUM(ACTOR_TYPE_PLAYER);
	REG_ENUM(ACTOR_TYPE_PET);
	REG_ENUM(ACTOR_TYPE_NPC);
#undef REG_ENUM


#define REG_ENUM(nmspc,name) lua_pushinteger(L, nmspc::name);	lua_setfield(L, -2, #nmspc#name);
	REG_ENUM(Action, Idle);
	REG_ENUM(Action, Walk);
	REG_ENUM(Action, Sit);
	REG_ENUM(Action, Angry);
	REG_ENUM(Action, Sayhi);
	REG_ENUM(Action, Dance);
	REG_ENUM(Action, Salute);
	REG_ENUM(Action, Clps);
	REG_ENUM(Action, Cry);
	REG_ENUM(Action, Batidle);
	REG_ENUM(Action, Attack);
	REG_ENUM(Action, Cast);
	REG_ENUM(Action, Behit);
	REG_ENUM(Action, Runto);
	REG_ENUM(Action, Runback);
	REG_ENUM(Action, Defend);
#undef REG_ENUM


	lua_pop(L, 1);


}
