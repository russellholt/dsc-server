#ifndef _DIS_SOCKET_H_
#define _DIS_SOCKET_H_ 1

/* A socket class
 *
 * Russell Holt, created May 4 1995
 * Destiny Software Corporation
 */

#include <stream.h>
#include <String.h>

extern "C" {
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
int listen(int, int);
int close(int);
int wait3(int *, int, struct rusage *);
int wait4(int, int *, int, struct rusage *);
int socket(int, int, int);
int bind(int, struct sockaddr *, int);
int accept(int, struct sockaddr *, int *);
}

#include <SLList.h>
#include <String.h>
#include "stringpair.h"
#include <stdio.h>

#define MAXLINE 1024
#define BUFSIZE 16384

class String;
class stringpair;

// netascii line termination sequence (excluding 0) -
// the null is so it can be used as a C string.

static char crlf[3] = { 0x0d, 0x0a, 0 };

class Socket {
protected:
	int					port;
	int					sockfd;	// socket file descriptor
	struct sockaddr_in	the_addr;
	//	in_addr *cli_sin_addr;
	int bytes_written, bytes_read;
	char internalBuffer[BUFSIZE];	// for reading

public:
	Socket(int a_port, int domain = AF_INET,
		int type = SOCK_STREAM, int protocol = 0);
	Socket(void);

	void Set(int sfd, int p, struct sockaddr_in *cli);

	inline void Listen(int queuesize=5)
		{ listen(sockfd, queuesize); }

	int Bind(void);
	void Accept(Socket *sock);

	void SetNonBlocking(void);
	int AcceptNonBlocking(Socket *sock);

	void Connect(void);
	inline void Close(void)
		{ close(sockfd); }

	inline int BytesWritten(void)
	{ return bytes_written; }

	inline int BytesRead(void)
	{ return bytes_read; }


	void SendStringList(SLList<String>& list);
	int SendAsciiFile(const char *filename);	// was Send_A_File
	int SendBinaryFile(const char *filename);	// was Send_A_File2
	int writen(const char *bufptr, const int nbytes);

	void ReadString(String &string);
	String *ReadAll(void);
	int readn(char *buf, int bufsize);
	int readline(char *ptr, const int bufsize);
	void die(const char *string);

	inline int id(void)
		{ return sockfd; }
	char *RemoteIP(void);
	char *RemoteHost(void);
	struct hostent *hostbyaddr(void);
	inline int Port(void) { return port; }
	
	// cool input and output
	friend inline Socket& operator<<(Socket& sock, String& string);

	friend inline Socket& operator<<(Socket& sock, const char *Cstring);

	friend inline Socket& operator>>(Socket& sock, String& string);

	friend inline Socket& operator>>(Socket& sock, char *buf);

};


/*
 * Write a string to a socket
 * Russell Holt, May 4 1995
 */
inline Socket& operator<<(Socket& sock, String& string)
{
    int w = sock.writen(string.chars(), string.length());
	return sock;
}


inline Socket& operator<<(Socket& sock, char *Cstring)
{
	int w = sock.writen(Cstring, strlen(Cstring));
    return sock;
}


/* the >> (get-from) operator for Socket
 *
 * Returns a Socket, so you can do mySocket >> string1 >> string2 ..
 *
 * The number of chars read for each String is its length().
 *
 * Russel Holt, May 4 1995
 */
inline Socket& operator>>(Socket& sock, String& string)
{
int l=BUFSIZE;
	string = "";
	while (l == BUFSIZE)	// handle lines greater than BUFSIZE
	{
    	l = sock.readline(sock.internalBuffer, BUFSIZE);
    	string += sock.internalBuffer;
	}
	return sock;
}

/*
 * >> operator 2: more efficient, but less stringy
 * ASSUME buf is allocated and of size MAXLINE.
 * Returns the number of chars read.
 */
inline Socket& operator>>(Socket& sock, char *buf)
{
    int l = sock.readline(buf, MAXLINE);
    return sock;
}


#endif
