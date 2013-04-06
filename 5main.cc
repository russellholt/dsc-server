#include "httpserver.h"

main(int argc, char *argv[])
{
httpserver *TheServer = new httpserver();

	TheServer->CommandLine(argc, argv);
	TheServer->Setup();
	TheServer->SocketSetup();	// from class Server

	//	TheServer->ListenLoop();
	TheServer->Run();

}
