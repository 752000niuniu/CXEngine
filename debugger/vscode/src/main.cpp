#include "debugger_adapter.h"
#include "kbase/at_exit_manager.h"


int main(int argc, char* argv[])
{
	kbase::AtExitManager exit_manager;
	debugger_adapter_init(argc, argv);
	debugger_adapter_run();
	return 0;
}

