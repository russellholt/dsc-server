#ifndef _HTTP_REQUEST_H_
#define _HTTP_REQUEST_H_

#include <stream.h>
#include <String.h>
#include <SLList.h>
#include "Socket.h"
#include "SLListSock.h"
//	#include "httpResHandler.h"

#include "cmd.h"

class httpResHandler;
class Socket;
class HTTP;

/*
 * httprequest
 * Purpose: to parse and store both the request data and information
 * derived from it.
 *
 * Russell Holt July 25-28, 1995
 */
class httprequest {

	SLList<String> Request;	// original text request

//--- request headers ---

	String	Request_Line,
			From,
			UserAgent,	// name of the web browser being used
			Referer;	// URL of the document containing the requested URL

	SLList<String> Accept;	// usually one "Accept:" header per type.

	SLList<String> others;	// any other request fields present, such as:
			// Accept-Encoding, Accept-Language,
			// Authorization, If-Modified-Since ...

//--- extracted & learned information ---

	String	URL,
			Command,	// text version of the command
			Path,
			extra_path,
			extra_path_real,
			HTTP_Version,
			document_root,
			cgidata,
			mime_type;

	int	cgi_pos,		// position in url where cgi data starts
		extra_path_pos,	// position of first '$' - extra path info.
		doingCGI,
		cmd,		// integer value for the command (from http.h)
		directory,	// URL references a directory
		show_request;

public:
	httprequest(void);
	void print(void);
	void ParseRequest(void);
	void ParseRequestLine(void);
	void ParseURL(void);
	void AddDocRoot(String &old, String& New);

	inline void NewURL(String& u) { URL = u; ParseURL(); }

	inline void append(String& s) {	Request.append(s); }

	inline void Debug(char *s)
	{
#ifdef DEBUG
		cerr << s;
#endif
	}
	inline void Debug(String& s)
	{
#ifdef DEBUG
		cerr << s << '\n';
#endif
	}

	friend class HTTP;
	friend class httpResHandler;
	friend inline Socket& operator <<(Socket& sock, httprequest& r);
};

//====================================================================

/*
 *	Send the request over a socket.
 */
inline Socket& operator <<(Socket& sock, httprequest& r)
{
	sock << r.Request_Line << crlf;

	if (r.From.length())
		sock << r.From << crlf;

	if (r.Referer.length())
		sock << r.Referer << crlf;

	if (r.UserAgent.length())
		sock << r.UserAgent << crlf;

	sock << r.Accept;
	sock << r.others;
	sock << crlf;	// signals end of the request headers.

	return sock;
}

#endif
