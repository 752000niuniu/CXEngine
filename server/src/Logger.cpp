#include "Logger.h"
#include "script_system.h"
#include "utils.h"

void log_print( char *str) 
{
	printf("%s\n", utils::Utf8ToGB2312(str).c_str());
}

void luaopen_logger(lua_State* L)
{
	script_system_register_function(L, log_print);
}