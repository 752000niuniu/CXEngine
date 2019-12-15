#include <script_system.h>
#include <script_system.inl>

#include "define_types.h"
#include "singleton.h"
#include "file_system.h" 
#include "logger.h"
#include "window.h"

#include "file_loading.h"
#include "imgui/cximgui.h"
#include "net_thread_queue.h"
#include "lua_net.h"
#include "luadbg.h"

#ifdef SIMPLE_ENGINE
#include "game.h"
#include "ui.h"
#include "text_renderer.h"
#include "input_manager.h"
#include "net.h"
#include "animation/sprite.h"
#include "resource_manager.h"
#include "sprite_renderer.h"
#include "animation/frame_animation.h"
#include "audio/audio_manager.h"
#include "graphics/ui_renderer.h"
#endif

#ifdef SIMPLE_SERVER
#include "server.h"
#endif // SIMPLE_SERVER

#ifndef CXLUAX
#include "time/time_manager.h"
#include "time/timer_manager.h"
#include "scene/scene_manager.h"
#include "scene/scene.h"
#include "protocol.h"
#include "combat/combat.h"
#include "actor/action.h"
#include "actor/actor_manager.h"
#endif // !CXLUAX
#include <cxlua.h>
#include <tsv.h>

static lua_State* L = nullptr;
void luaopen_script_system(lua_State* L);


using GameConfig = std::map<std::string, std::string>;
GameConfig g_GameConfig;

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
	luaopen_filesystem(L);
	luaopen_script_system(L);
	luaopen_cximgui(L);
	luaopen_logger(L);
	luaopen_ne_support(L);
	luaopen_window(L);
#ifndef CXLUAX
	luaopen_timer_manager(L);
	luaopen_actor(L);
	luaopen_actor_manager(L);
	luaopen_scene(L);
	luaopen_scene_manager(L);
	luaopen_protocol(L);
	luaopen_cxlua(L);
#endif // !CXLUAX
#ifdef SIMPLE_ENGINE
	luaopen_game(L);
	luaopen_resource_manager(L);
	luaopen_input_manager(L);
	luaopen_ui(L);
	luaopen_sprite_renderer(L);
	luaopen_text_renderer(L);
	luaopen_frame_animation(L);
	luaopen_sprite(L);
	luaopen_net(L);
	luaopen_combat_system(L);
	luaopen_audio_manager(L);
	luaopen_ui_renderer(L);
	luaopen_action(L);
#endif
#ifdef SIMPLE_SERVER
	luaopen_game_server(L);
#endif // !SIMPLE_SERVER

	int res = luaL_loadbuffer(L, script_system_lua_code, strlen(script_system_lua_code), "__script_system_lua_code__");
	check_lua_error(L, res);
	res = lua_pcall(L, 0, LUA_MULTRET, 0);
	check_lua_error(L, res);
}

 

void script_system_run_main_script()
{
	int res = luaL_dofile(L, FileSystem::GetLuaPath("main.lua").c_str());
	if (!check_lua_error(L, res, FileSystem::GetLuaPath("main.lua").c_str()))
	{
		DebugBreak();
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

bool process_is_server(){
#ifdef SIMPLE_SERVER
	return true;
#else
	return false;
#endif
}

void luaopen_script_system(lua_State* L)
{

#define REG_ENUM(name, macro)  (lua_pushinteger(L, macro),lua_setglobal(L, name))
#ifdef SIMPLE_SERVER
	REG_ENUM("SIMPLE_SERVER",SIMPLE_SERVER);
#endif
#ifdef SIMPLE_ENGINE
	REG_ENUM("SIMPLE_ENGINE", SIMPLE_ENGINE);
#endif
#ifdef CXLUAX
	REG_ENUM("CXLUAX", CXLUAX);
#endif
#undef REG_ENUM

	script_system_register_function(L, script_system_get_config);
	script_system_register_function(L, lua_file_path);
	script_system_register_function(L, process_is_server);
}