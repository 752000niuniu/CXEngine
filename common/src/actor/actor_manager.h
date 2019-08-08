#pragma once

#include "actor.h"
#include "player.h"


Player* actor_manager_fetch_local_player();

Player* actor_manager_find_player_by_name(const char* name);

Player* actor_manager_create_player(uint64_t pid);

void actor_manager_update();
void actor_manager_draw();

void luaopen_actor_manager(lua_State* L);
