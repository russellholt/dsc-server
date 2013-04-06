#include "Server.h"
#include <sys/wait.h>

void Server::Init(void)
{
	block_io = 0;
}

Server::Server(void) : Port("Port", SERV_TCP_PORT),
	host_name("HostName",""),
	referer_log("RefererLog","")
{
	Init();
}

void Server::CommandLine(int argc, char **argv)
{
int c;
GetOpt getopt(argc, argv, "p:");

    while ((c = getopt()) != EOF)
        switch(c)
        {
            case 'p':
                Port = atoi(getopt.optarg);
                break;
            case 'h':
            case '?':
                cerr.form("Usage: %s [-p port]\n", argv[0]);
                exit(1);
            default: ;
        }
}

/*
 * Open a socket
 */
void Server::SocketSetup(void)
{
	Debug("Opening stream socket...");

    ListenSock = NULL;
    ClientSock = NULL;

    ListenSock = new Socket(Port.intval(), AF_INET, SOCK_STREAM, 0);
    if (!ListenSock)
        die("Unable to create socket");

    ClientSock = new Socket(Port.intval(), AF_INET, SOCK_STREAM, 0);
    if (!ClientSock)
        die("Unable to create a client socket");

	Debug("Success\nBinding...");

    if (ListenSock->Bind() < 0)
        die("Unable to bind to address.");

    Debug("Success.\n");

    ListenSock->Listen();
}

/* ListenLoop
 *  Wait for a new connection.
 *  fork() to handle it with NewConnection()
 *
 * Russell Holt, May 9 1995
 *
 * Modified for wait3 (get rid of <defunct> processes)
 *   on July 5 1995 RFH
 */
void Server::ListenLoop(void)
{
int childpid;
int childstat, childstatpid;

	for (;;)
    {
		Debug("Waiting for new connection...\n");

        // Wait for a connection from a client process.
        ListenSock->Accept(ClientSock);

		Debug("Accepted\n");

        if ((childpid = fork()) < 0)
            die("Server: fork error");
        else
            if (childpid == 0)          // child process
            {
				NewConnection();
				break;
			}
		else	// parent
		{
			// -- this only kills the previous child, so that the
			//    one that was just forked won't get  killed till the
			//    *next* request.
			childstatpid = wait3(&childstat, WNOHANG, NULL);

#ifdef DEBUG
				cerr << "(server::ListenLoop) Child status for "
					 << childstatpid << '\n';
#endif
		}

        ClientSock->Close();    // but do not delete!

    }
}


/* NewConnection
 *  Do stuff when a client connects
 *  Must be overridden by subclass
 */
void Server::NewConnection(void)
{
	Debug("Server::NewConnection()\n");
}

/* NonBlockListenLoop
 *  Wait for a new connection, but do other things while we wait.
 *  The big idea is that while we wait we can service current requests
 *  without forking, by breaking down the job of request proccessing
 *  into small tasks, and create a new server subclass object (like http)
 *  for each connection.  During the wait loop we might execute the next
 *  small task for each server object.  This is also the way that we
 *  might listen on multiple ports.
 */
void Server::NonBlockListenLoop(void)
{
int childpid, got_it=0;

	ListenSock->SetNonBlocking();

int a = 10000;
int childstat, childstatpid;
int nprocs = 0;
int lastchildpid = -1;
	for(;;)
	{
		got_it = 0;
		Debug("Waiting for new connection (non-blocking)...\n");

		do {	// busy wait.
			got_it = ListenSock->AcceptNonBlocking(ClientSock);
			nprocs++;
			/* to listen on another port at the same time we might
			 * call got_it2 = ListenSock2-> ...
			 */
			if (lastchildpid > 0 && nprocs && --a == 0)	// only ask for status if there are child processes
			{
				//	childstatpid = wait3(&childstat, WNOHANG, NULL);
				childstatpid = wait4(lastchildpid, &childstat, WNOHANG, NULL);

				if (childstatpid > 0)
				{
#ifdef DEBUG
						cerr << "(server::ListenLoop) Child status for "
							<< childstatpid << '\n';
#endif

					nprocs--;	// a process was terminated
					lastchildpid = -1;
				}
				a = 10000;
			}

		} while (!got_it);

		Debug("Accepted\n");

        if ((childpid = fork()) < 0)
		{
			perror("[Destiny] Server: fork error - ");
			_exit(1);
		}
        else
            if (childpid == 0)          // child process
                NewConnection();

        ClientSock->Close();    // but do not delete!

		lastchildpid = childpid;

	}

}

void Server::Run(void)
{
	ListenLoop();
}
