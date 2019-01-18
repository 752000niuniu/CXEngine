#pragma once
#include "lua_bind.h"

void script_system_read_config(int argc, char** argv);

void script_system_prepare_init();

void script_system_dofile(const char* file);

std::string script_system_get_config(std::string key);

void script_system_init();
void script_system_update();
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


