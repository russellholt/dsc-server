/* class cgi
 *
 * For decoding CGI data and/or loading it via the GET or POST method
 * from an HTTP server.
 *
 *	If the cgi data is already loaded (as inside the server), use
 *
 *		cgi abc(encoded_CGI_data);
 *
 *	If the cgi data needs to be obtained from the environment (ie, the GET
 *	method) or from the command line (ie, the POST method), use
 *
 *		cgi abc;
 *		abc.LoadData();
 *
 *	Then do something like:
 *
 *		stringpair name_value_list;
 *		abc.ParseCGIData(name_value_list);
 *
 *  Then name_value_list is a list of stringpairs, where the left element
 *	is the name and the right element is the value.
 *
 * Russell Holt, June/July 1995
 */
#ifndef _CGI_H_
#define _CGI_H_ 1

#include <stream.h>
extern "C"
{
#include <stdio.h>
#include <stdlib.h>
}
#include <String.h>
#include <SLList.h>
#include "stringpair.h"

// environment variables set by the server
#define N_ENV_VARS 18

static char server_env[N_ENV_VARS][20] = {
	"SERVER_SOFTWARE",
	"SERVER_NAME",
	"GATEWAY_INTERFACE",
	"SERVER_PROTOCOL",
	"SERVER_PORT",
	"REQUEST_METHOD",
	"SCRIPT_NAME",
	"REMOTE_HOST",
	"REMOTE_ADDR",
	"AUTH_TYPE",
	"REMOTE_USER",
	"CONTENT_LENGTH",
	"HTTP_ACCEPT",
	"HTTP_USER_AGENT",
	"PATH_INFO",
	"PATH_TRANSLATED",
	"REMOTE_IDENT",
	"CONTENT_TYPE"
};


static Regex esc_hex("%[0-9a-fA-F][0-9a-fA-F]");	// percent followed by two hex digits
static String hexdig = "0123456789abcdef";

class cgi {
	String cgi_data;
	SLList<stringpair> *thelist;
public:
	cgi(String *data);
	cgi(char *data);
	cgi(void);

	int LoadData(void);
	void Get(void);
	void Post(void);
	void Test(void);

	inline void SendType(char *type)
	{
		printf("Content-type: %s%c%c", type, 10, 10);
	}

	void ParseCGIData(SLList<stringpair>& cgilist);
	String unescape_hex(String h);
	void GetServerVars(SLList<stringpair>& var_list);
};


#endif
