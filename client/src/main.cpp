#include "window.h"
#include "script_system.h"
#include "kbase/at_exit_manager.h"
#include "ezio/io_service_context.h"

int main(int argc, char const *argv[])
{
	kbase::AtExitManager exit_manager;
	ezio::IOServiceContext::Init();

	script_system_read_config(argc, argv);

	script_system_prepare_init();

	script_system_dofile("main.lua");
	
	script_system_call_function(script_system_get_luastate(),"main");
	
	return 0;
}
