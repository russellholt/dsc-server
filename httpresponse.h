#ifndef _HTTP_RESPONSE_H_
#define _HTTP_RESPONSE_H_

#include <String.h>
#include <SLList.h>
#include "Socket.h"
#include "SLListSock.h"
//	#include "http.h"
#include "status_codes.h"

//	class HTTP;
class Socket;

/* These fields in these classes are intended to be accessed directly
 * from within the HTTP class.
 */


class httpresponse {

	String response_line;

// Other Response-Headers
	String	current_date,
			server,
			mime,
			last_mod_date,
			content_type,
			content_length,
			status_phrase,
			location;

	SLList<String> others;	// extensibility!

	int status_code,
		bytes_sent,
		add_mod_date;

// general information about the response
	String logmessage;


public:
	httpresponse(void);
	void CreateResponseLine(String& http_version);
	void print(void);

	friend class HTTP;
	friend class httpResHandler;
	friend inline Socket& operator <<(Socket& sock, httpresponse& r);
};


/*
 *	Send the response over a socket.
 */
inline Socket& operator <<(Socket& sock, httpresponse& r)
{
		// crlf defined in Socket.h
	sock << r.response_line << crlf;
	sock << r.current_date << crlf;
	sock << r.server << crlf;
	sock << r.mime << crlf;

	if (r.last_mod_date.length())
		sock << r.last_mod_date << crlf;

	if (r.content_type.length())
		sock << r.content_type << crlf;

	if (r.content_length.length())
		sock << r.content_length << crlf;
	
	if (r.location.length())
		sock << "Location: " << r.location << crlf;

	sock << r.others;

	// Here, sending an additional crlf would signal the end of the headers.
	// We can't do that because additional headers may need to be sent,
	// such as the Content-type which is sent by CGI programs.

	return sock;
}

#endif
