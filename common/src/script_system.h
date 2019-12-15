#pragma once

#include "lua_bind.h"

void script_system_read_config(int argc, char const *argv[]);

void script_system_prepare_init();

void script_system_run_main_script();


std::string script_system_get_config(const char* key);

void script_system_init();
bool script_system_update();
void script_system_draw();
void script_system_deinit();

lua_State* script_system_get_luastate();

any script_system_get_globle(const char* name);

template <class T>
T script_system_get_globle(const char* name)
{
	any v = script_system_get_globle(name);
	return any_cast<T>(v);
}

template <typename T>
T* lua_check_pointer(lua_State* L, int index){
	T** ptr = (T**)lua_touserdata(L, index);
	return *ptr;
};

template <typename T>
void lua_push_pointer(lua_State* L, T* ptr) {
	T** ud = (T**)lua_newuserdata(L, sizeof(T*));
	*ud = ptr;
}
