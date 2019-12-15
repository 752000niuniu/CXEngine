#include "server.h"
#include "tsv.h"
#include "ezio/socket_address.h"
#include "kbase/at_exit_manager.h"
#include "ezio/io_service_context.h"
#include <script_system.h>
#include "file_system.h"




int main(int argc, char const *argv[])
{
	kbase::AtExitManager exit_manager;
	ezio::IOServiceContext::Init();
	script_system_read_config(argc, argv);
	
	FileSystem::InitWorkPath();
	script_system_prepare_init();
	script_system_run_main_script();
	script_system_init();
	game_server_start(45000);
	script_system_deinit();
	return 0;
}
