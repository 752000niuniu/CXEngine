#include "script_system.h"
#include "lua_bind.h"
#include "define_types.h"
#include "lua_net.h"
#include "debugger_adapter.h"

static lua_State* L = nullptr;
void luaopen_script_system(lua_State* L);

#define luaL_requirelib(L,name,fn) (luaL_requiref(L, name, fn, 1),lua_pop(L, 1))
extern "C" int luaopen_cjson(lua_State *L);

using GameConfig = std::map<std::string, std::string>;
GameConfig g_GameConfig;

void script_system_read_config(int argc, char** argv)
{
	
}

void script_system_prepare_init()
{
	L = luaL_newstate();
	luaL_openlibs(L);
	luaL_requirelib(L, "cjson", luaopen_cjson);
	luaopen_script_system(L);
	luaopen_netlib(L);
}

void script_system_dofile(const char* file)
{
	if (luaL_dofile(L, file) != LUA_OK)
	{
		const char* msg = lua_tostring(L, -1);
		printf("%s\n", msg);
		return;
	}
}

std::string script_system_get_config(std::string key)
{
	auto it = g_GameConfig.find(key);
	if (it != g_GameConfig.end())
	{
		return it->second;
	}
	else
	{
		return "";
	}
}

void script_system_init()
{
}

void script_system_update()
{
}

void script_system_draw()
{
}

void script_system_deinit()
{
}

lua_State* script_system_get_luastate()
{
	return L;
}

any script_system_get_globle(const char* name)
{
	lua_getglobal(L, name);
	any v = lua_getanyvalue(L, -1);
	lua_pop(L, 1);
	return v;
}

void luaopen_script_system(lua_State* L)
{
	script_system_register_function(L, script_system_dofile);
}