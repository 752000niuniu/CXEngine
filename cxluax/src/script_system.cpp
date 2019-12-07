#include <script_system.h>

#include <define_types.h>
#include <singleton.h>
#include <file_system.h>
#include <logger.h>
#include <file_loading.h>
#include <imgui/cximgui.h>
#include <net_thread_queue.h>
#include <lua_net.h>
#include <luadbg.h>
#include <cxlua.h>
#include <tsv.h>
#include "window.h"

static lua_State *L = nullptr;
void luaopen_script_system(lua_State *L);

#define luaL_requirelib(L, name, fn) (luaL_requiref(L, name, fn, 1), lua_pop(L, 1))
extern "C" int luaopen_cjson(lua_State *L);

using GameConfig = std::map<std::string, std::string>;
GameConfig g_GameConfig;

void script_system_read_config(int argc, char const *argv[])
{
	g_GameConfig["argv0"] = argv[0];
	for (int i = 1; i < argc; i++)
	{
		std::string argi = argv[i];
		cxlog_info("%s\n", argi.c_str());

		if (argi.substr(0, 2) == "--")
		{
			argi = argi.substr(2, argi.size());
			auto kv = utils::split(argi, '=');
			g_GameConfig[kv[0]] = kv[1];
		}
		else if (argi.substr(0, 1) == "-")
		{
			argi = argi.substr(1, argi.size());
			g_GameConfig[argi] = "";
		}
	}
}

void script_system_prepare_init()
{
	L = luaL_newstate();
	luaL_openlibs(L);
	luaopen_luadbg(L);
	luaL_requirelib(L, "cjson", luaopen_cjson);
	luaopen_filesystem(L);
	luaopen_script_system(L);
	
	luaopen_cximgui(L);
	luaopen_logger(L);
	
	
	luaopen_window(L);
	luaopen_cxlua(L);
}

void script_system_dofile(const char *file)
{
	int res = luaL_dofile(L, FileSystem::GetLuaPath(file).c_str());
	if (!check_lua_error(L, res, FileSystem::GetLuaPath(file).c_str()))
	{
		DebugBreak();
	}
}

std::string script_system_get_config(const char *key)
{
	auto it = g_GameConfig.find(key);
	if (it != g_GameConfig.end())
	{
		return it->second;
	}
	else
	{
		return "nil";
	}
}

void script_system_init()
{
	script_system_call_function(L, "on_script_system_init");
}

bool script_system_update()
{
	std::vector<any> rets = script_system_call_function(L, "on_script_system_update");
	bool success = any_cast<bool>(rets[0]);
	return success;
}

void script_system_draw()
{
	script_system_call_function(L, "on_script_system_draw");
}

void script_system_deinit()
{
	script_system_call_function(L, "on_script_system_deinit");
}

lua_State *script_system_get_luastate()
{
	return L;
}

any script_system_get_globle(const char *name)
{
	lua_getglobal(L, name);
	any v = lua_getanyvalue(L, -1);
	lua_pop(L, 1);
	return v;
}

const char *lua_file_path(const char *luafile)
{
	static std::string v;
	v = FileSystem::GetLuaPath(luafile);
	return v.c_str();
}

void luaopen_script_system(lua_State *L)
{

#define REG_ENUM(name, macro) (lua_pushinteger(L, macro), lua_setglobal(L, name))
#undef REG_ENUM

	script_system_register_function(L, script_system_dofile);
	script_system_register_function(L, script_system_get_config);

	script_system_register_function(L, lua_file_path);
}