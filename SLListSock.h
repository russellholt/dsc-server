/*
 * Operators for SLList templations
 *
 * Russell Holt July 1995
 *
 */

#ifndef _SLLIST_SOCK_H_
#define _SLLIST_SOCK_H_

#include <stream.h>
#include <String.h>
#include <SLList.h>
#include "Socket.h"
#include "stringpair.h"

class Socket;
class stringpair;
class String;

// Send a SLList<String> over the socket
inline Socket& operator<<(Socket& sock, SLList<String> list)
{
Pix temp = list.first();
String a;
int w;

	while(temp)
	{
		a = list(temp) + crlf;
		w = sock.writen(a.chars(), a.length());
		list.next(temp);
	}
	return sock;
}

// Send a list of stringpairs over a socket
inline Socket& operator<<(Socket& sock, SLList<stringpair> list)
{
Pix temp = list.first();
stringpair a;
String l, r;
int w;

	while(temp)
	{
		a = list(temp);
		l = a.left(); r = a.right();

		w = sock.writen("(", 1);
		w = sock.writen(l, l.length());
		w = sock.writen(", ", 2);
		w = sock.writen(r, r.length());
		w = sock.writen(")\n", 2);

		list.next(temp);
	}

	return sock;
}

#endif
