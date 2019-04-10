#include "actor.h"
#include "action.h"
#include "player.h"
#include "scene/scene_manager.h"
#include "utils.h"
#include "lua.hpp"

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

void player_set_frame_speed(int frame_speed)
{
	Player* player = SCENE_MANAGER_INSTANCE->GetCurrentScene()->GetLocalPlayer();
	if (player) {
		player->SetFrameSpeed(frame_speed);
	}
}

void player_set_move_speed(int move_speed)
{
	Player* player = SCENE_MANAGER_INSTANCE->GetCurrentScene()->GetLocalPlayer();
	if (player) {
		player->SetVelocity(move_speed);
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


luaL_Reg mt_actor[] = {
	{ "SetProperty",actor_method_set_property },
{ "GetProperty",actor_method_get_property },
{ "Destroy",actor_destroy },
{ NULL, NULL }
};


void lua_push_actor(lua_State*L, Actor* actor)
{
	Actor** ptr = (Actor**)lua_newuserdata(L, sizeof(Actor*));
	*ptr = actor;
	if (luaL_newmetatable(L, "mt_actor")) {
		luaL_setfuncs(L, mt_actor, 0);
		lua_pushvalue(L, -1);
		lua_setfield(L, -2, "__index");
	}
	lua_setmetatable(L, -2);
}

Actor* lua_check_actor(lua_State*L, int index)
{
	Actor** ptr = (Actor**)lua_touserdata(L, index);
	return *ptr;
}


int lua_new_actor(lua_State* L)
{
	int type = luaL_optinteger(L, 1, ACTOR_TYPE_DEFAULT);
	int rold_id = luaL_optinteger(L, 2, 0);
	
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
	if (luaL_newmetatable(L, "mt_actor")) {
		luaL_setfuncs(L, mt_actor, 0);
		lua_pushvalue(L, -1);
		lua_setfield(L, -2, "__index");
	}
	return 1;
}


void luaopen_actor(lua_State* L)
{
	script_system_register_function(L, player_set_frame_speed);
	script_system_register_function(L, player_set_move_speed);
	script_system_register_luac_function(L, lua_new_actor);
	script_system_register_luac_function(L, actor_get_metatable);
	
	lua_rawgeti(L, LUA_REGISTRYINDEX, LUA_RIDX_GLOBALS);
#define REG_ENUM(e) lua_pushinteger(L, e);	lua_setfield(L, -2, #e);
	REG_ENUM(ACTOR_TYPE_DEFAULT);
	REG_ENUM(ACTOR_TYPE_PLAYER);
	REG_ENUM(ACTOR_TYPE_PET);
	REG_ENUM(ACTOR_TYPE_NPC);
#undef REG_ENUM
	lua_pop(L, 1);

	
}
