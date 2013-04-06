#ifndef _DIS_SERVER_H
#define _DIS_SERVER_H 1

#include "unixsystem.h"
#include <stream.h>
#include <GetOpt.h>
#include <String.h>
#include "Socket.h"
#include "../crb/R_Integer.h"
#include "../crb/R_String.h"

#define SERV_TCP_PORT 3141
#define SERV_HOST_ADDR "204.120.87.10"

class Server {
protected:
//	int port;
	R_Integer Port;
	Socket *ListenSock, *ClientSock;
	int block_io;
	R_String host_name, referer_log;
	void Init(void);
	
public:
	Server(void);
	void CommandLine(int argc, char **argv);
	virtual void Setup(void) { }
	void SocketSetup(void);
	void ListenLoop(void);
	void NonBlockListenLoop(void);
	virtual void NewConnection(void);
	void die(char *string)
		{ cerr << string << '\n'; exit(1); }
	void Run(void);

	inline String HostName(void) { return host_name.Value(); }
	inline String RefererLog(void) { return referer_log.Value(); }
	
	inline void Debug(char *str)
	{
#ifdef DEBUG
		cerr << str;
#endif
	}
};

#endif
