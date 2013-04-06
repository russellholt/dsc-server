#ifndef _HTTPRESHANDLER_H_
#define _HTTPRESHANDLER_H_

#include <stream.h>
#include <String.h>
#include <SLList.h>

#include "stringpair.h"
#include "resHandler.h"
#include "http.h"
#include "Mime.h"
//	#include "metaurl.h"
#include "surl.h"
#include "unixsystem.h"
#include "httprequest.h"
#include "httpserver.h"
#include "cgi.h"
#include "R_Surlist.h"

#ifndef DESTINY_URL
#define DESTINY_URL "http://www.destinyusa.com/"
#endif

//	class Socket;
//	class cgi;
class HTTP;
class httprequest;
class httpresponse;
class httpserver;

class httpResHandler : public resHandler {
	httprequest *Request;
	httpresponse *Response;
	httpserver *server;
	HTTP *protocol;
	Mime *MimeInfo;
	char mod_date[80];
//	metaurl *MetaURL;
	surl *TheSurl;

public:
	httpResHandler(void);
	httpResHandler(httprequest *request, httpresponse *response,
		HTTP *a_protocol, Mime *mime, Socket *sock, httpserver *serv);
	int Get(void);
	int CGI(void);

	int GetURLInfo(String& cont_type, String& Modification_Date,
		String& cont_length, int& trans_mode);

	void DirectoryIndex(void);

	inline char *url_mod_date(void)
		{ return mod_date; }

	inline void Mod_Message(void)
		{
			*Sock << "<p><i>Last Modified " << mod_date << "</i>";
		}

	void Destiny_Message(void);

	int ExternalCGI(void);

	inline void Debug(char *str)
	{
#ifdef DEBUG
		cerr << str;
#endif
	}

	inline void Debug(String& str)
	{
#ifdef DEBUG
		cerr << str;
#endif
	}

	inline void Debug(char *str1, String& str2)
	{
#ifdef DEBUG
		cerr << str1 << str2 << '\n';
#endif
	}
	inline void Debug(char *str1, int a, char *str2 = " ")
	{
#ifdef DEBUG
		cerr << str1 << a << str2;
#endif
	}
};

#endif
