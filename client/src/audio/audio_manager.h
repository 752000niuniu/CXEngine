#pragma once

int audio_manager_play(const char* path);
void luaopen_audio_manager(lua_State* L);