#include <iostream>
extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include "lualib.h"
}

#define luaL_requirelib(L,name,fn) (luaL_requiref(L, name, fn, 1),lua_pop(L, 1))
extern "C" int luaopen_cjson(lua_State *L);
extern  void luaopen_debugger(lua_State* L);

int main()
{
	lua_State * L = luaL_newstate();
	luaL_openlibs(L);
	luaL_requirelib(L, "cjson", luaopen_cjson);
	luaopen_debugger(L);
	
	luaL_dofile(L, "D:\\Github\\SimpleEngine\\debugger\\test\\main.lua");
	

	return 0;
}