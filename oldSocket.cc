#include "Socket.h"
#include <stdio.h>
#include <ctype.h>

extern "C" {
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
int read(int, char *, long unsigned int);
int write(int, const char *, long unsigned int);
char *inet_ntoa(struct in_addr);
void bzero(char *, int);
void bcopy(const char *, char *, int);
}

Socket::Socket(int a_port, int domain, int type, int protocol)
{
	sockfd = socket(domain, type, protocol);
	if (sockfd < 0)
		die("unable to create stream socket.");
	port = a_port;

	bytes_written = bytes_read = 0;
}

Socket::Socket(void)
{
	bytes_written = bytes_read = 0;
}

char *Socket::RemoteIP(void)
{
	return inet_ntoa(the_addr.sin_addr);
}

// Return the hostname for the connected client
char *Socket::RemoteHost(void)
{
struct hostent *hstuff;

	hstuff = gethostbyaddr((char *) (&the_addr.sin_addr),
		sizeof(the_addr.sin_addr), AF_INET);

	if (hstuff)
		return hstuff->h_name;
}

// when you want more specific info
struct hostent *Socket::hostbyaddr(void)
{
	return gethostbyaddr((char *) (&the_addr.sin_addr),
		sizeof(the_addr.sin_addr), AF_INET);
}

int Socket::Bind(void)
{
	bzero((char *) &the_addr, sizeof(the_addr));
	the_addr.sin_family = AF_INET;
	the_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	the_addr.sin_port = htons(port);

	return (bind(sockfd, (struct sockaddr *) &the_addr, sizeof(the_addr)));
}

/*
 *
 */
void Socket::Accept(Socket *newsock)
{
struct sockaddr_in *cli_addr = new sockaddr_in;
int clilen = sizeof(sockaddr_in);
int newsockfd = -1;

	newsockfd = accept(sockfd, (struct sockaddr *) cli_addr,
					&clilen);
	if (newsockfd > -1)
		if (newsock)
		{
			// find out IP connect is coming from
		/*
			cli_sin_addr = new in_addr;
			bcopy(cli_addr.sin_addr, cli_sin_addr, sizeof(cli_addr.sin_addr));
			//	String b = inet_ntoa(cli_sin_addr);
		*/

			String b = inet_ntoa(cli_addr->sin_addr);
#ifdef DEBUG
			cerr << "\n===== Accept connection from " << b << " ======\n";
#endif

			newsock->Set(newsockfd, port, cli_addr);
		}
		else
			die("Socket: accept error- no new Socket!");
}

void Socket::Set(int sfd, int p, struct sockaddr_in *cli)
{
	sockfd = sfd;
	port = p;
	bcopy((char *) cli, (char *) (&the_addr), sizeof(sockaddr_in));
}


void Socket::SetNonBlocking(void)
{
int a = 0;

	int flags = fcntl(sockfd, F_GETFL, a);
	cout << "fcntl flags are: " << flags << "  a is " << a << '\n';
	//	fcntl(sockfd, F_SETFL, flags + FNDELAY);
	fcntl(sockfd, F_SETFL, FNDELAY | FAPPEND );

	//	a = ioctl(sockfd, FIONBIO, (char *) &a);
}

/*
 *  Use a non-blocking IO socket accept call.
 *  Need to call SetNonBlock first.
 *  Return 0 if there is no pending connection.
 *  Set newsock and return 1 if connection has been accepted.
 *
 * Russell Holt, May 12 1995
 */
int Socket::AcceptNonBlocking(Socket *newsock)
{
struct sockaddr_in *cli_addr = new sockaddr_in;
int clilen = sizeof(sockaddr_in);
int newsockfd = -1;

	newsockfd = accept(sockfd, (struct sockaddr *) cli_addr,
                &clilen);

	if (newsockfd > -1)
	{
		if (newsock)
		{
			newsock->Set(newsockfd, port, cli_addr);
			return 1;
		}
		else
			die("Socket: AcceptNonBlocking error - invalid Socket parameter");
	}

	if (errno == EWOULDBLOCK)
		return 0;

	return 0;
}

void Socket::Connect(void)
{

}

/*  Send a binary file over a socket.
 *
 */
int Socket::SendBinaryFile(const char *filename)
{
FILE *fp = 0;
int nread = 0, nwritten, bufsize = BUFSIZE, bytes_sent = 0;
char *buf = new char[BUFSIZE];

    // open it
    fp = fopen(filename, "r");
    if (!fp)
	{
		char x[256];
		sprintf(x, "Could not open input file %s\n", filename);
		perror(x);
		die("");
	}

    //  while((nread = read(fp, buf, bufsize) > 0)
    for (;;)
    {
        bytes_sent += nread = read(fileno(fp), buf, bufsize);
#ifdef DEBUG
        printf("Socket::SendBinaryFile: Read %d bytes\n", nread);
#endif
        if (nread < 1)
            break;
        nwritten = writen(buf, nread);
    }

    delete buf;
    fclose(fp);
    bytes_written += bytes_sent;	// add to total
    
	return bytes_sent;
}

/*
 *  Send an ascii file over the socket
 */
int Socket::SendAsciiFile(const char *filename)
{
FILE *fp;
int n, bytes_sent = 0;
char sendline[MAXLINE];

    // open it
    fp = fopen(filename, "r");
    if (!fp)
	{
		char x[256];
		sprintf(x, "Could not open input file %s\n", filename);
		perror(x);
		die("");
	}

    while (fgets(sendline, MAXLINE, fp) != NULL)
    {
        bytes_sent += n = strlen(sendline);
        if (writen(sendline, n) != n)
            die("str_cli: writen error on socket");
    }
    fclose(fp);
	bytes_written += bytes_sent;	// add to total
	return bytes_sent;
}

/*
 *  Write n bytes to the socket
 *  Basically from _Unix_Network_Programming_ by W. Richard Stevens.
 */
int Socket::writen(const char *bufptr, const int nbytes)
{
int nleft, nwritten;

    nleft = nbytes;

    while (nleft > 0) {
        nwritten = write(sockfd, bufptr, nleft);
        if (nwritten <=0)
            return (nwritten);

        nleft -= nwritten;
        bufptr += nwritten;
    }
    bytes_written += (nbytes-nleft);
    return (nbytes - nleft);
}

void Socket::ReadString(String &string)
{
char buf[2048];	// 2 k
	

}

/*
 * ASSUME buf has already been allocated.
 *  Returns the actual number of chars read.
 */
int Socket::readn(char *buf, int bufsize)
{
	int l = read(sockfd, buf, bufsize);
	bytes_read += l;
	return l;
}

/*
 *  Read an ascii `line' from the socket
 *  It is null terminated
 */
int Socket::readline(char *ptr, const int bufsize)
{
int n, rc;
char c;

    for (n=1; n<bufsize; n++)
    {
        if ((rc = read(sockfd, &c, 1)) == 1)
        {
            *ptr++ = c;
            if (c == '\n')
                break;
        } else
            if (rc == 0) {
                if (n == 1)
                    return (0);     /* EOF, no data read */
                else
                    break;          /* EOF, some data read */
            } else
                return (-1);        /* error */
    }
    *ptr = 0;   /* terminate */
    bytes_read += n;
    return (n);
}


/*
 ***** **** *** ** * *  *   *    *     *
 *
 * Send an SLList<String> over the socket, and be sure to remove
 * any special or weird characters that might happen to be in the
 * Strings
 */
void Socket::SendStringList(SLList<String>& list)
{
Pix temp = list.first();
String a;
char *b=0, *ptr=0;
int x,y,w;
char c;

    while(temp)
    {
        a = list(temp);
		if (b)
			delete b;
		ptr = b = new char[a.length() + 1];

        x = a.length() - 1;
		for(y=0; c=a[y], y<x; y++)
			if (isprint(c)) *ptr++ = c;
		*ptr = 0;	// null terminate (not necessary for writen tho)

		w = writen(b, (int) (ptr-b));
		bytes_written += (int) (ptr-b) + 4;	// 4 for the "<br>"
		w = writen("<br>", 4);
        list.next(temp);
    }
}


void Socket::die(const char *string)
{
	cerr << '\n' << string << '\n';
	exit(1);
}


/* ReadAll
 *
 *  Read everything off the socket and put it into one big string.
 *  Return the string.  I'm using a String because it can be of
 *  arbitrary size - it does the data handling internally, and it
 *  is easier to manipulate than a char *.  
 *
 * Russell Holt, July 18, 1995

String *Socket::ReadAll()
{
String *bigstring = new String;
char temp[BUFSIZE];
int size, totalsize=0;

	// read the whole thing
	for(;;)
	{
		size = readn(temp, BUFSIZE);

		if (size <= 0)
			break;

		bigstring->append(temp);
	}

	return bigstring;
}
*/
