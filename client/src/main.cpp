#include "window.h"
#include <script_system.h>
#include "kbase/at_exit_manager.h"
#include "ezio/io_service_context.h"
#include "file_system.h"
#include "cxlua.h"

int main(int argc, char *argv[])
{
	handle_command_args(argc,argv);
	FileSystem::InitWorkPath();

	kbase::AtExitManager exit_manager;
	ezio::IOServiceContext::Init();
	script_system_prepare_init();
	script_system_run_main_script();
	return 0;
}
