#include "window.h"
#include "script_system.h"
#include "kbase/at_exit_manager.h"
#include "ezio/io_service_context.h"
#include "file_system.h"
#include "audio/audio_manager.h"

int main(int argc, char const *argv[])
{
	script_system_read_config(argc, argv);
	
	kbase::AtExitManager exit_manager;
	ezio::IOServiceContext::Init();
	FileSystem::InitWorkPath();
	script_system_prepare_init();
	script_system_dofile("main.lua");
	//script_system_call_function(script_system_get_luastate(),"main");
	audio_manager_play(R"(f:\Github\SimpleEngine\res\data\sound.wdf.ResFiles\00E5E050.wav)");
	return 0;
}


