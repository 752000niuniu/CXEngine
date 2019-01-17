#include "Logger.h"
#include "script_system.h"
#include "kbase/string_util.h"

void log_print(const char *str) 
{
	printf("%s\n", utils::Utf8ToGB2312(str).c_str());
}

void luaopen_logger(lua_State* L)
{
	script_system_register_function(L, log_print);
}