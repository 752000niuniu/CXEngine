#include "actor_manager.h"
#include "actor.h"
#include "actor/player.h"
#include "window.h"
#include "scene/scene_manager.h"

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

int lua_actor_manager_fetch_local_player(lua_State*L)
{
	auto it = g_Players.find(g_LocalPid);
	if (it != g_Players.end()) {
		lua_push_actor(L, it->second);
		return 1;
	}
	return 0;
}

Player* actor_manager_fetch_local_player()
{
	auto it = g_Players.find(g_LocalPid);
	if (it != g_Players.end()) {
		return dynamic_cast<Player*>(it->second);
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
			int screenWidth = WINDOW_INSTANCE->GetWidth();
			int screenHeight = WINDOW_INSTANCE->GetHeight();
			int halfScreenWidth = screenWidth / 2;
			int halfScreenHeight = screenHeight / 2;
			int mapWidth = scene->GetGameMap()->GetWidth();
			int mapHeight = scene->GetGameMap()->GetHeight();

			int px = localPlayer->GetX();
			int py = localPlayer->GetY();

			int maxMapOffsetX = mapWidth - halfScreenWidth;
			int maxMapOffsetY = mapHeight - halfScreenHeight;

			px = px < halfScreenWidth ? px :
				(px > maxMapOffsetX ?
				(screenWidth - (mapWidth - px)) : halfScreenWidth);
			py = py < halfScreenHeight ? py :
				(py > maxMapOffsetY ?
				(screenHeight - (mapHeight - py)) : halfScreenHeight);
			if (SCENE_MANAGER_INSTANCE->IsDrawStrider())
				localPlayer->OnDraw(px, py);
			if (SCENE_MANAGER_INSTANCE->IsDrawMask()) {
				scene->GetGameMap()->DrawMask(localPlayer->GetX(), localPlayer->GetY(), dynamic_cast<Player*>(localPlayer)->GetDrawY());
			}

		}
		else {
			player->OnDraw(scene->GetGameMap());
		}
	}
#endif // !SIMPLE_SERVER
}
int lua_actor_manager_create_player(lua_State*L)
{
	uint64_t pid = (uint64_t)lua_tointeger(L, 1);
	Player* player = new Player(pid);
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

void luaopen_actor_manager(lua_State* L) {
	script_system_register_luac_function_with_name(L, "actor_manager_create_actor", lua_actor_manager_create_actor);

	script_system_register_luac_function_with_name(L, "actor_manager_create_player", lua_actor_manager_create_player);
	
	script_system_register_luac_function_with_name(L, "actor_manager_fetch_local_player", lua_actor_manager_fetch_local_player);

	script_system_register_luac_function_with_name(L, "actor_manager_fetch_player_by_id", lua_actor_manager_fetch_player_by_id);

	
	script_system_register_luac_function_with_name(L, "scene_manager_fetch_local_player", lua_actor_manager_fetch_local_player);

	script_system_register_luac_function_with_name(L, "actor_manager_fetch_all_players", lua_actor_manager_fetch_all_players);
	

	script_system_register_luac_function(L, actor_manager_set_local_player);
	

}

