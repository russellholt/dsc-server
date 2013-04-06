#ifndef _HTTP_H_
#define _HTTP_H_

#include <stream.h>
#include <String.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

#include "Socket.h"
#include "Mime.h"
#include "httpResHandler.h"
// #include "metaurl.h"
#include "httpserver.h"
#include "httprequest.h"
#include "httpresponse.h"
#include "SLListSock.h"
#include "SLList_util.h"

#include "R_Surlist.h"

#include "unixsystem.h"

/*
 * HTTP protocol class
 *
 * Russell Holt, may 11-16 1995
 */

#include "cmd.h"
#include "status_codes.h"

// content transfer modes
#define MODE_ERROR -2
#define MODE_NONE -1
#define MODE_BINARY 0
#define MODE_ASCII 1
#define MODE_HTML 2
#define MODE_META 3

// HTTP response header strings
#define CONTENT_TYPE "Content-type: "
#define LAST_MODIFIED "Last-modified: "
#define CONTENT_LENGTH "Content-length: "

#ifndef DESTINY_HTTPD_VERS
#define DESTINY_HTTPD_VERS "0.04"
#endif

#ifndef DESTINY_URL
#define DESTINY_URL "http://204.120.87.10:3141/dest.html"
#endif

class httpresponse;
class httpResHandler;
class httprequest;

class httpserver;

class HTTP {
	Socket *Sock;
	httpserver *server;
	Mime *MimeInfo;
	int meta_url, trans_mode;
//	int metaurlOnly;
	char mod_date[80];
	httprequest Request;
	httpresponse Response;
	httpResHandler *rhandler;

public:

	HTTP(void);

	void Init(Socket *s, String doc_root,
		Mime *TheMime, httpserver *serv);

// Main methods -------------------------------------------

	void Handle_Request(void);

	int _processrequest(void);
	int ProcessRequest(void);

	void SendResponse(void);
	void SendContent(String& filename);

// Utility methods -----------------------------------------

	int DateFormat(time_t *thetime, char *buf, int buflen);
	String *HTTPCurrentTimeDate(void);
	void BounceRequestResponse(void);
	void die(const char *string);

	void HTTPStatusMessage(void);

	// Call before sending any data over the socket - sends the
	// HTTP response headers contained in the class data member
	// 'Response'.  Could also be used for sending encryption
	// information etc.
	inline void BeginDataTransfer(void)
	{	SendResponse();	}

	// Call when data transfer is complete.  Does nothing now,
	// but could be used for sending encryption information.
	inline void EndDataTransfer(void)
	{ }
};


#endif
