#include "server.h"
#include "tsv.h"
#include "ezio/socket_address.h"
#include "kbase/at_exit_manager.h"
#include "ezio/io_service_context.h"
#include "script_system.h"


ezio::EventLoop loop;
void main_game_update() {
	if (!script_system_update()) {
		loop.Quit();
	}
}

int main(int argc, char const *argv[])
{
	kbase::AtExitManager exit_manager;
	ezio::IOServiceContext::Init();

	script_system_read_config(argc, argv);

	script_system_prepare_init();

	script_system_dofile("main.lua");
	
	script_system_init();

	loop.RunTaskEvery(main_game_update, TimeDuration(16));
	loop.Run();

	script_system_deinit();

	
	return 0;
}
