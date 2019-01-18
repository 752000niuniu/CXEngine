#include "script_system.h"

#include "defineTypes.h"
#include "Singleton.h"
#include "file_system.h" 
#include "Logger.h"
#include "Window.h"
#include "scene/SceneManager.h"
#include "ui.h"
#include "ne_wrapper.h"
#include "TextRenderer.h"
#include "InputManager.h"
#include "net.h"
#include "debugger.h"
#include "wrap_imgui_impl.h"
#include "net_thread_queue.h"
#include "lua_net.h"
#include "scene/Scene.h"

static lua_State* L = nullptr;
void luaopen_script_system(lua_State* L);

#define luaL_requirelib(L,name,fn) (luaL_requiref(L, name, fn, 1),lua_pop(L, 1))
extern "C" int luaopen_cjson(lua_State *L);

using GameConfig = std::map<std::string, std::string>;
GameConfig g_GameConfig;

void script_system_read_config(int argc, char** argv)
{
	for (int i = 1; i < argc; i++)
	{
		std::cout << argv[i] << std::endl;
		auto kv = utils::split(argv[i], '=');
		if (kv.size() == 2)
		{
			g_GameConfig[kv[0]] = kv[1];
		}
		else if (kv.size() == 1)
		{
			g_GameConfig[kv[0]] = "";
		}
	}
}

void script_system_prepare_init()
{
	L = luaL_newstate();
	luaL_openlibs(L);
	luaL_requirelib(L, "cjson", luaopen_cjson);
	luaopen_script_system(L);
	luaopen_imgui(L);
	luaopen_logger(L);
	luaopen_ne_support(L);
	luaopen_window(L);
	luaopen_resource_manager(L);
	luaopen_timer_manager(L);
	luaopen_input_manager(L);
	luaopen_ui(L);
	luaopen_sprite_renderer(L);
	luaopen_text_renderer(L);
	luaopen_scene(L);
	luaopen_scene_manager(L);
	luaopen_frame_animation(L);
	luaopen_tsv(L);
	luaopen_net_thread_queue(L);
	luaopen_netlib(L);
	luaopen_net(L);
	luaopen_debugger(L);
}

void script_system_dofile(const char* file)
{
	if (luaL_dofile(L, FileSystem::GetLuaPath(file).c_str()) != LUA_OK)
	{
		const char* msg = lua_tostring(L, -1);
		LOG_ERR(msg);
		return;
	}
}

const char* script_system_get_config(const char* key)
{
	auto it = g_GameConfig.find(key);
	if (it != g_GameConfig.end())
	{
		return it->second.c_str();
	}
	else
	{
		return "";
	}
}

void script_system_init()
{
	script_system_call_function(script_system_get_luastate(),"on_script_system_init");
}

void script_system_update()
{
	script_system_call_function(script_system_get_luastate(),"on_script_system_update");
}

void script_system_draw()
{
	script_system_call_function(script_system_get_luastate(),"on_script_system_draw");
}

void script_system_deinit()
{
	script_system_call_function(script_system_get_luastate(),"on_script_system_deinit");
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

void luaopen_script_system(lua_State* L)
{
	script_system_register_function(L, script_system_dofile);
	script_system_register_function(L, script_system_get_config);
}