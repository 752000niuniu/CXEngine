#include "actor_manager.h"
#include "actor.h"
#include "actor/player.h"
#include "window.h"
#include "scene/scene_manager.h"
#include "scene/base_scene.h"
#include "scene/game_map.h"
#include "utils.h"
#include "cxmath.h"

std::map<uint64_t, Actor*> g_Players;
uint64_t g_LocalPid = 0;

int actor_manager_set_local_player(lua_State*L) {
	uint64_t pid = (uint64_t)lua_tointeger(L, 1);
	g_LocalPid = pid;
	return 0;
}

bool actor_manager_is_local_player(Actor* actor) {
	return actor->GetID() == g_LocalPid;
}

void actor_manager_set_scene(int id)
{
	for(auto& it : g_Players){
		it.second->SetSceneID(id);
	}
}
int lua_actor_manager_fetch_local_player(lua_State*L)
{
	auto it = g_Players.find(g_LocalPid);
	if (it != g_Players.end()) {
		lua_push_actor(L, it->second);
		return 1;
	}
	return 0;
}

Actor* actor_manager_fetch_local_player()
{
	auto it = g_Players.find(g_LocalPid);
	if (it != g_Players.end()) {
		return it->second;
	}
	return nullptr;
}

Player* actor_manager_find_player_by_name(const char* name){
	for(auto& it : g_Players){
		if(it.second->GetName() ==std::string(name)){
			return dynamic_cast<Player*>(it.second);
		}
	}
	return nullptr;
}

void actor_manager_update()
{
	for (auto& it : g_Players)
	{
		it.second->OnUpdate();
	}
}

void actor_manager_draw()
{
#ifndef SIMPLE_SERVER

	auto* localPlayer = actor_manager_fetch_local_player();
	if (localPlayer == nullptr)return;

	BaseScene* scene = scene_manager_fetch_scene(localPlayer->GetSceneID());
	if (scene == nullptr || scene->GetGameMap() == nullptr)return;

	for (auto& it : g_Players)
	{
		Actor* player = it.second;
		if (localPlayer == player)
		{
			if (SCENE_MANAGER_INSTANCE->IsDrawStrider()) {
				localPlayer->OnDraw();
			}
			if (SCENE_MANAGER_INSTANCE->IsDrawMask()) {
				scene->GetGameMap()->DrawMask(localPlayer->GetX(), localPlayer->GetY(), (localPlayer)->GetY());
			}
		}
		else {
			Pos lpos = localPlayer->GetPos();
			Pos pos = player->GetPos();
			if (GMath::Astar_GetDistanceSquare(lpos.x, lpos.y, pos.x, pos.y) <= 400 * 400) {
				player->OnDraw();
			}
		}
		
	}
#endif // !SIMPLE_SERVER
}
int lua_actor_manager_create_player(lua_State*L)
{
	uint64_t pid = (uint64_t)lua_tointeger(L, 1);
	Actor* player = new Actor(pid);
	g_Players.insert({ pid, player });
	lua_push_actor(L, player);
	return 1;
}

int lua_actor_manager_create_actor(lua_State*L)
{
	uint64_t pid = (uint64_t)lua_tointeger(L, 1);
	Actor* actor = new Actor(pid);
	g_Players.insert({ pid, actor });
	lua_push_actor(L, actor);
	return 1;
}


Player* actor_manager_create_player(uint64_t pid) {
	if(g_Players.find(pid) != g_Players.end()){
		delete g_Players[pid];
		g_Players.erase(pid);
	}
	Player* player = new Player(pid);
	g_Players.insert({ pid, player });
	return player;
}

int lua_actor_manager_fetch_player_by_id(lua_State*L) {
	uint64_t pid = (uint64_t)lua_tointeger(L, 1);
	auto it = g_Players.find(pid);
	if (it != g_Players.end()) {
		lua_push_actor(L, it->second);
		return 1;
	}
	return 0;
}


int lua_actor_manager_fetch_all_players(lua_State*L) {
	lua_newtable(L);
	int i = 1;
	for(auto& it : g_Players){
		lua_push_actor(L, it.second);
		lua_seti(L, -2, i++);
	}
	return 1;
}

int lua_actor_manager_clear_all(lua_State*L){
	for (auto& it : g_Players) {
		SafeDelete(it.second);
	}
	g_Players.clear();
	return 0;
}

void luaopen_actor_manager(lua_State* L) {
	
	script_system_register_luac_function_with_name(L, "actor_manager_clear_all", lua_actor_manager_clear_all);

	script_system_register_luac_function_with_name(L, "actor_manager_create_actor", lua_actor_manager_create_actor);

	script_system_register_luac_function_with_name(L, "actor_manager_create_player", lua_actor_manager_create_player);
	
	script_system_register_luac_function_with_name(L, "actor_manager_fetch_local_player", lua_actor_manager_fetch_local_player);

	script_system_register_luac_function_with_name(L, "actor_manager_fetch_player_by_id", lua_actor_manager_fetch_player_by_id);

	script_system_register_luac_function_with_name(L, "scene_manager_fetch_local_player", lua_actor_manager_fetch_local_player);

	script_system_register_luac_function_with_name(L, "actor_manager_fetch_all_players", lua_actor_manager_fetch_all_players);
	
	script_system_register_function(L, actor_manager_set_scene);
	script_system_register_luac_function(L, actor_manager_set_local_player);
	

}

