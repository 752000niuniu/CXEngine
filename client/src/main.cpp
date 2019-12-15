#include "window.h"
#include <script_system.h>
#include "kbase/at_exit_manager.h"
#include "ezio/io_service_context.h"
#include "file_system.h"

int main(int argc, char const *argv[])
{
	script_system_read_config(argc, argv);
	
	kbase::AtExitManager exit_manager;
	ezio::IOServiceContext::Init();
	FileSystem::InitWorkPath();
	script_system_prepare_init();
	script_system_run_main_script();
	return 0;
}
