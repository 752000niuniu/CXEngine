#include "server.h"
#include "tsv.h"
#include "ezio/socket_address.h"
#include "kbase/at_exit_manager.h"
#include "ezio/io_service_context.h"
#include <script_system.h>
#include "file_system.h"


int main(int argc, char const *argv[])
{
	script_system_read_config(argc, argv);
	FileSystem::InitWorkPath();
	script_system_prepare_init();
	script_system_dofile("main.lua");
	game_main_run();
	return 0;
}
