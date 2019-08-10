#include "script_system.h"

#include "define_types.h"
#include "singleton.h"
#include "file_system.h" 
#include "logger.h"
#include "window.h"
#include "scene/scene_manager.h"
#include "file_loading.h"
#include "imgui/cximgui.h"
#include "net_thread_queue.h"
#include "lua_net.h"
#include "scene/scene.h"
#include "time/time_manager.h"
#include "luadbg.h"
#include "time/timer_manager.h"
#include "game.h"
#include "actor/actor_manager.h"

#ifndef SIMPLE_SERVER
#include "ui.h"
#include "text_renderer.h"
#include "input_manager.h"
#include "net.h"
#else
#include "server.h"
#endif // !SIMPLE_SERVER
#include "protocol.h"
#include "combat/combat.h"




static lua_State* L = nullptr;
void luaopen_script_system(lua_State* L);

#define luaL_requirelib(L,name,fn) (luaL_requiref(L, name, fn, 1),lua_pop(L, 1))
extern "C" int luaopen_cjson(lua_State *L);

using GameConfig = std::map<std::string, std::string>;
GameConfig g_GameConfig;

void check_lua_error(lua_State* L, int res)
{
	if (res != LUA_OK) { printf("%s\n", lua_tostring(L, -1)); }
}

void script_system_read_config(int argc, char const *argv[])
{
	g_GameConfig["argv0"] = argv[0];
	for (int i = 1; i < argc; i++)
	{
		std::string argi = argv[i];
		cxlog_info("%s\n", argi.c_str());

		if(argi.substr(0,2) == "--"){
			argi = argi.substr(2, argi.size());
			auto kv = utils::split(argi, '=');
			g_GameConfig[kv[0]] = kv[1];
		}else if(argi.substr(0, 1) == "-") {
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
	luaopen_tsv(L);
	luaopen_cximgui(L);
	luaopen_logger(L);
	luaopen_ne_support(L);
	luaopen_window(L);
	luaopen_timer_manager(L);
	luaopen_scene(L);
	luaopen_scene_manager(L);
	luaopen_actor(L);
	luaopen_actor_manager(L);
	luaopen_game(L);
	luaopen_net_thread_queue(L);
	luaopen_netlib(L);
	luaopen_protocol(L);
#ifndef SIMPLE_SERVER
	luaopen_resource_manager(L);
	luaopen_input_manager(L);
	luaopen_ui(L);
	luaopen_sprite_renderer(L);
	luaopen_text_renderer(L);
	luaopen_frame_animation(L);
	luaopen_net(L);
	luaopen_combat_system(L);
#else
	luaopen_game_server(L);
#endif // !SIMPLE_SERVER
}

void script_system_dofile(const char* file)
{
	if (luaL_dofile(L, FileSystem::GetLuaPath(file).c_str()) != LUA_OK)
	{
		const char* msg = lua_tostring(L, -1);
		cxlog_err(msg);
		DebugBreak();
		return;
	}
}

std::string script_system_get_config(const char* key)
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
	script_system_call_function(L , "on_script_system_init");
}

bool script_system_update()
{
	std::vector<any> rets = script_system_call_function(L, "on_script_system_update");
	bool success = any_cast<bool>(rets[0]);
	return success;
}


void script_system_draw()
{
	script_system_call_function(L,"on_script_system_draw");
}

void script_system_deinit()
{
	script_system_call_function(L,"on_script_system_deinit");
}

lua_State* script_system_get_luastate ()
{
	return L;
}

any script_system_get_globle(const char* name)
{
	lua_getglobal(L, name);
	any v = lua_getanyvalue(L,-1);
	lua_pop(L, 1);
	return v;
}

const char* lua_file_path(const char* luafile)
{
	static std::string v;
	v = FileSystem::GetLuaPath(luafile);
	return v.c_str();
}

void luaopen_script_system(lua_State* L)
{

#define REG_ENUM(name, macro)  (lua_pushinteger(L, macro),lua_setglobal(L, name))
#ifdef SIMPLE_SERVER
	REG_ENUM("SIMPLE_SERVER",SIMPLE_SERVER);
#else
	REG_ENUM("SIMPLE_ENGINE", SIMPLE_ENGINE);
#endif // SIMPLE_SERVER
#undef REG_ENUM

	script_system_register_function(L, script_system_dofile);
	script_system_register_function(L, script_system_get_config);

	script_system_register_function(L, lua_file_path);
}