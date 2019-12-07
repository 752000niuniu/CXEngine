#include <script_system.h>
#include <file_system.h>
#include <ezio/event_loop.h>
#include <ezio/tcp_server.h>
#include <kbase/at_exit_manager.h>
#include <ezio/io_service_context.h>

ezio::EventLoop g_MainLoop;
int main(int argc, char const *argv[])
{
	script_system_read_config(argc, argv);
	FileSystem::InitWorkPath();
	script_system_prepare_init();
	script_system_dofile("main.lua");
	script_system_init();
	g_MainLoop.RunTaskEvery([]() {
		if (!script_system_update()) {
			g_MainLoop.Quit();
		}
	}, ezio::TimeDuration(16));
	g_MainLoop.Run();
	script_system_deinit();
	return 0;
}
