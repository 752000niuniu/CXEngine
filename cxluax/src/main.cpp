#include <script_system.h>
#include <file_system.h>
#include <ezio/event_loop.h>
#include <ezio/tcp_server.h>
#include <kbase/at_exit_manager.h>
#include <ezio/io_service_context.h>
#include "cxlua.h"
#include "net_thread_queue.h"


int main(int argc, char  *argv[])
{
	handle_command_args(argc, argv);

	FileSystem::InitWorkPath();
	script_system_prepare_init();
	script_system_run_main_script();
	
	return 0;
}
