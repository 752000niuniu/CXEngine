#pragma once

extern "C"
{
#include <lua.h>
}

void luaopen_debugger(lua_State* L);