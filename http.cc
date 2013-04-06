/*
 * HTTP
 *
 * History of Changes
 *
 * Version 0.5 -----------------------------
 * July 24-28: major re-structure
 * - old ProcessRequest is now _processrequest.
 * - Incorporated class httprequest.
 * - Moved ParseRequestLine, ParseURL, ParseRequest to class httprequest.
 *
 * Version 0.4 ----------------------------
 * June 21: began attempt to de-linearize and generalize ProcessRequest,
 *    so that it is more flexible and so we can do things like get a
 *    new url from the output of a CGI program and restart the "request".
 * - incorporated new class 'httpresponse' (instead of StringSLList)
 *
 * May 17: httpResHandler (resource handler) class works,
 * - moved GetURLInfo to httpResHandler
 *
 * May 16: created resource handler class (doesn't work yet)
 * - added HTTPErrorMessage
 * ...
 *
 * May 10: Beginning CGI parsing
 *
 * May 9: incorporated use of Mime class
 *
 * Cinco de Mayo: fully incorporated Sockets class
 * - all writes to the socket now use the form MySocket << stuff
 *   including writing a StringSLList.
 * - removed the stupid ReqLine struct.
 *
 * May 4: rewrote, shuffled, & created request.h:
 * - rewrote GetURLInfo and added the actual URL modification date
 *   and now returns a response code (like 200 = "OK")
 * - wrote HTTPDateFormat
 * - ProcessRequest adds the current date to the Response
 * - ProcessRequest constructs the Response line
 * - Slightly incorporated Sockets class
 * - Created HTTP class (where these things now live)
 *
 * May 3: wrote all HTTP specific routines, got them working
 *    (basic HTTP server working)
 * - wrote ProcessRequest
 * - wrote SendContent
 * - wrote SendResponse
 * - Used the libg++ GetOpts class and added command line options
 *
 * May 2: wrote everything from scratch
 * - basic socket code modified from examples in
 *   _Unix_Network_Programming_ by W. Richard Stevens
 * - recompiled with g++
 * - began using the libg++ String class
 * - generated StringSLList class with `genclass'
 * - added routines to read lines from a socket to Strings which
 *   get added to a StringSLList
 *
 * Russell Holt
 * Destiny Software Corporation
 */

#include "http.h"
#include "SLListSock.h"
#include "SLList_util.h"

extern "C" {
#include <sys/ioctl.h>
#include <stdio.h>

time_t time(time_t *tloc);
int getpid(void);
}

FILE *fap;

HTTP::HTTP(void)
{
	Sock = NULL;
	server = NULL;
}

void HTTP::Init(Socket *s, String doc_root, Mime *TheMime, httpserver *serv)
{
	if (!s)
		die("HTTP::Init : Socket is NULL!!");

	Sock = s;
	MimeInfo = TheMime;
//	meta_url = 0;
	Response.add_mod_date = 1;
	server = serv;
	Request.document_root = doc_root;
}

/* Handle_Request
 * Read the request from the socket by:
 * - read each line and turn it into a String
 * - add each String to a StringSLList (a linked list of Strings)
 * - when there's nothing left, hand the StringSLList off to ProcessRequest
 *
 * Russell Holt May 2,3 1995
 */
void HTTP::Handle_Request(void)
{
int		n, response_len = 0, pid = getpid();
char	response[MAXLINE + 80];	/* max length of response */
String line;
Socket& sock = *Sock;	// a convenience


    for (;;)
    {
		sock >> line;	// line gets null terminated.
		n = line.length();

		if (n && n < 3)
			if (line[0] = '\n' || (line[0] == 0x0d && line[1] == 0x0a))
			{
				ProcessRequest();
				sock.Close();
				return;
			}

		// Some of these tests could probably be taken out or
		// simplified upon detailed low-level testing. 

        if (n == 0)		// unexpected in request mode (!cli_mode)
		{
#ifdef DEBUG
			cerr << "Server process [" << pid;
			cerr << "]: Client closed connection, exiting.\n";
#endif

            return;     /* connection terminated */
		}
        else
            if (n < 0)
                die("Handle_Request: readline error");

		// remove a trailing newline
		if (line[n-1] == '\n')
			line[n-1] = '\0';

		Request.append(line);	// add input line to list
    }
}

/* ProcessRequest
 * new interface to old _processrequest (was ProcessRequest)
 *
 * Russell Holt, July 28 1995
 */
int HTTP::ProcessRequest(void)
{
int done = 0;

	Request.ParseRequest();
	
	// Create a resource handler
	rhandler = new httpResHandler(&Request, &Response, this, MimeInfo, Sock, server);

	// setup the request-independent response headers

	Response.server = String("Server: Destiny/") + DESTINY_HTTPD_VERS;
	Response.mime = String("MIME-version: 1.0");
	String *tdate = HTTPCurrentTimeDate();
	Response.current_date = String("Date: ") + *tdate;

	while(!done)
		done = _processrequest();

	fclose(fap);
}

int HTTP::_processrequest(void)
{
String	Content_Type,
		Modification_Date,
		Content_Length;

	trans_mode = MODE_HTML;

	// Obtain URL information from the resource handler
	// should set the Response variables inside GetURLInfo() instead
	// of returning them like this.
	Response.status_code = rhandler->GetURLInfo(Content_Type, Modification_Date,
			Content_Length, trans_mode /*, server?server->MetaUrlOnly():0*/);

	Request.mime_type = Content_Type;
	// log this request, format like NCSA:
	//   hostname: [current time] "request_line" status_code


	Response.CreateResponseLine(Request.HTTP_Version);

	if (!Request.doingCGI)	// not doing CGI
	{
		if (trans_mode > MODE_NONE && Modification_Date.length())
			Response.last_mod_date = String(LAST_MODIFIED) + Modification_Date;

		Response.content_type = String(CONTENT_TYPE) + Content_Type;

		if (trans_mode == MODE_BINARY && Content_Length.length())
			Response.content_length = String(CONTENT_LENGTH) + Content_Length;
	}

	//  Called from ReadyToSendData(), invoked from httpResHandler methods
	//	SendResponse();

	
	


	
	
	
	
	
int done = 1;	// default is to do the GET (or whatever) and then be done!
				// but if GET returns 0, that implies that it modified
				// fields in the Request object, which will then have to
				// be re-processed (Yay!)

	if (trans_mode > MODE_NONE)
		switch(Request.cmd)
		{
			case CMD_GET:
				done = rhandler->Get();
				break;

			default:
				BeginDataTransfer();
				HTTPStatusMessage();
				EndDataTransfer();
		}
	else
	{
		BeginDataTransfer();
		HTTPStatusMessage();
		EndDataTransfer();
	}

	
	
/*
	char *fub = new char[512];
	char *fub2 = new char[256];
	sprintf(fub2, "%s %s", Request.Command.chars(), Request.URL.chars());
	sprintf(fub, "%s: [%s] \"%s\" %d %s<br>\n", Sock->RemoteHost(), server->Now(),
		fub2, Response.status_code, Response.logmessage.chars());
	server->Log(fub);
	delete fub;
	delete fub2;
*/
	
// do not log if the referrer is a local machine and the mime type is image.
// this means that requests for images on a page will not individually be logged.
//	char *local_host = new char[256];
//	gethostname(local_host, 256);	// can't use this because a DNS alias might point to the server
	int mm = setuid(0);
#ifdef DEBUG
	if (mm)
		cerr << "Setuid root failed\n";
#endif
	if (!(Request.Referer.contains(server->HostName()) && Request.mime_type.contains("image")))
	{
//		fap = fopen("/mongo/dest/server/acc_log.txt", "a");
		fap = fopen((server->AccessLogName()).chars(), "a");
		char *xx = new char[256];
		if (fap)
		{
			String nowow = server->Now();
			nowow.gsub("\n", "");
			sprintf(xx, "%s: [%s] \"%s %s\"", Sock->RemoteHost(), nowow.chars(),
				Request.Command.chars(), Request.URL.chars());
		
			fprintf(fap, "%s %d %d\n", xx,
				Response.status_code, Sock->BytesWritten());
// destiny log format - only difference is <br> and Response.logmessage
//			fprintf(fap, "%s %d %d %s<br>\n", xx,
//				Response.status_code, Sock->BytesWritten(), Response.logmessage.chars());
		}

#ifdef DEBUG
		else
			cerr << "Unable to open logfile.\n";
#endif

		if (Request.Referer != "" && !Request.Referer.contains(server->HostName()))
		{
			FILE *ref = fopen((server->RefererLog()).chars(), "a");
			if (ref)
			{
				fprintf(ref, "%s from <a href=%s>%s</a><br>\n", xx, Request.Referer.chars(),
					Request.Referer.chars());
				fclose(ref);
			}
		}
		delete xx;
	}	
	
	
	
	
	

	return done;
}

/*
 *  Make an ascii time string from the time struct to match the
 *  HTTP specs- it looks like: Friday, 28-Apr-95 20:35:10 GMT
 *  returns the actual length of the ascii time string
 *     (time_t is just a long)
 * Russell Holt, May 4 1995
 */
int HTTP::DateFormat(time_t *thetime, char *buf, int buflen)
{
	struct tm *tmtime = gmtime(thetime);	// convert it to a time struct
	int r = strftime(buf, buflen, "%A, %d-%B-%y %H:%M:%S GMT", tmtime);
	return r;
}


/*
 *  Send the response headers, not the content, to the client
 *  This is a linked list of Strings
 *
 * Russell Holt
 */
void HTTP::SendResponse(void)
{
#ifdef DEBUG
	cout << "\nThe Response is:\n";
	Response.print();
#endif

	*Sock << Response;

	// signal the end of the headers:
	// CGI processes are responsible for appending their own
	// "Content-type:" header, so there are more headers to come

	if (!Request.doingCGI)
		*Sock << crlf;	// CRLF defined in Socket.h
}

/*
 *  Send the actual data that the URL points to
 *  the URL should be parsed at this point, that is,
 *  modified accordingly.  It's not really the original
 *  requested URL then.  They both point to the same thing
 *  though
 *
 * Russell Holt
 */
void HTTP::SendContent(String& filename)
{
int bytes_sent;

	if (trans_mode)
	{
		bytes_sent = Sock->SendAsciiFile(filename.chars());
		if (trans_mode == MODE_HTML)
		{
			if (Response.add_mod_date)
			{
				rhandler->Mod_Message();
				rhandler->Destiny_Message();
			}

		}
		if (Request.show_request)
			BounceRequestResponse();
	}
	else
		bytes_sent = Sock->SendBinaryFile(filename.chars());

}


void HTTP::die(const char *string)
{
	cerr << "\n[Destiny httpd] Fatal error: HTTP: " << string << "\n";
	fprintf(fap, " *** fatal error %s\n", string);
	fclose(fap);
	exit(1);
}

/* HTTPCurrentTimeDate
 *  Create and return a String containing the current time and date
 *  in the HTTP specified format.
 *
 * Russell Holt, May 11 1995
 */
String *HTTP::HTTPCurrentTimeDate(void)
{
	time_t now = time(0);
	char today[80];		// will not be longer than this.
	int r = DateFormat(&now, today, 79);

		// "Date: " is the MIME header part... remove from here?
	String *TimeDateNow = new String(today);

	return TimeDateNow;
}

void HTTP::BounceRequestResponse()
{
	*Sock << "<h1>The Request</h1>"
		 << "Your browser sent the following request:<hr><pre>";

	Sock->SendStringList(Request.Request);	// original text

	*Sock << "</pre><hr>The Destiny server sent the "
		 << "following response:<hr><pre>";
	*Sock << Response;
	*Sock << "</pre><hr>";
}

/* send an error message over the socket
 * according to the status code.
 *
 * Actually there should be a 3 column table like this:
 *
 * status_code  phrase         filename
 * ------------------------------------
 * 200          OK             *
 * 400          Bad Request    bad_reqeust.html
 * 401          Unauthorized   unauthrorized.html
 * ...
 *
 * Which does what you think it should do instead of what
 * I'm doing here.
 *
 * Russell Holt May 16, 1995
 *
 */
void HTTP::HTTPStatusMessage(void)
{
String stat = dec(Response.status_code);

#ifdef DEBUG
	cout << "Logmessage: " << Response.logmessage << '\n';
#endif

	*Sock << "<html><head><title>Destiny HTTP Server Error</title></head>"
		 << "<body><h1>"
		 << stat << " "
		 << Response.status_phrase << "</h1><hr>";

	switch(Response.status_code)
	{
		case STATUS_NO_RESPONSE:		// 204
				break;
		case STATUS_MOVED:				// 301
				*Sock << "The requested URL <tt>" << Request.URL << "</tt> has moved to "
					  << "<a href=\"" << Response.location << "\">" << Response.location
					  << "</a>. Please update your links.<p>\n";
				break;
		case STATUS_BAD_REQUEST:		// 400
				Request.show_request = 1;
				break;
		case STATUS_UNAUTHORIZED:		// 401
				*Sock << "Actually, your browser should be asking you "
					 << "for authentication right now...";
				break;
		case STATUS_PAYMENT_REQUIRED:	// 402
				*Sock << "Hey, you don't get this for free...";
				break;
		case STATUS_FORBIDDEN:
				*Sock << "Sorry, access to the URL <tt>"
					<< Request.URL
					<< "</tt> has been <b>denied</b>.<p>";
					//	<< " is available to everyone but <b>you</b>.";
				break;
		case STATUS_NOT_FOUND:
				*Sock << "The Destiny server was unable to locate <tt>"
					<< Request.URL << "</tt>.";
				break;
		case STATUS_INTERNAL_ERROR:
				*Sock << "The Destiny Server experienced an internal "
					 << "error while attempting to access the URL "
					 << Request.URL << " ...<p>";
				 *Sock << "Please contact <a href=\"mailto:webmaster@destinyusa.com\">webmaster</a>"
					 << " and report your problem.<p>Thank you.";
				break;
		case STATUS_NOT_IMPLEMENTED:
				*Sock << "-34<font size=5>_b</font>( #0<b>x</b> & <i>3c</i> cxa}^!2"
					 << " is not currently implemented in the Destiny HTTP server.";
				break;
		default: ;
	}

	if (Request.show_request)
		BounceRequestResponse();

	//	Sock << crlf << crlf;
}
