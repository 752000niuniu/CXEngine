#include "defines.h"
#include "server.h"
#include "tsv.h"
#include "ezio/socket_address.h"
#include "kbase/at_exit_manager.h"
#include "ezio/io_service_context.h"

int main(int argc, char const *argv[])
{
	kbase::AtExitManager exit_manager;
	ezio::IOServiceContext::Init();


	int port = 45000;
	for (int i = 0; i < argc; i++)
	{
		auto params = utils::split_by_cnt(argv[i], '=', 2);
		if (params[0] == "--port")
		{
			if (params[1] != "")
			{
				port = std::stoi(params[1]);
			}
			else
			{
				port = 45000;
			}
		}
	}
	ezio::EventLoop loop;
	ezio::SocketAddress addr(port);
	GameServer server(&loop,addr,"GameServer");
	server.Start();
	loop.Run();
	return 0;
}
