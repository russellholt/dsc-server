// NAME: httprequest.cc
//
// PURPOSE:
//
// PROGRAMMING NOTES:
//
// HISTORY: 
//
// $Id$


#include "httprequest.h"
#include "SLList_util.h"
#include "cgi.h"

//==========================================================

httprequest::httprequest(void)
{
	doingCGI = cmd = directory = show_request = 0;
	extra_path_pos = -1;
	cgi_pos = -1;
}

void httprequest::print(void)
{
	cout << Request_Line << '\n';
	cout << From << '\n';
	cout << UserAgent << '\n';
	cout << Referer << '\n';

	cout << Accept << '\n';
	cout << others << '\n';
}


void httprequest::ParseRequest(void)
{
String URLbeforeCGI;


	// should be Request line.
	Request_Line = Request.front();

	ParseRequestLine();

#ifdef DEBUG
	cout << "\nThe Request is:\n";
	cout << Request << '\n';
	cout << "CMD_GET is " << CMD_GET << '\n';
	cout << "CMD_POST is " << CMD_POST << '\n';
	cout << "CMD_PUT is " << CMD_PUT << '\n';
	cout << "CMD_DELETE is " << CMD_DELETE << '\n';
#endif

	if (Command.length() <= 0)
	{
		cerr << "Error in httprequest::ParseRequest() - Failed to parse request line\n";
		_exit(1);
	}

	// Parse the rest of the request headers
	//  Like I've mentioned previously, this probably should be
	//  done in a general "headers" type of class where there
	//  is a table of the text of the expected headers and via
	//  subclassing - a mapping to class variables so that the
	//	parsing can happen "automatically" and not like this:

	Debug("Parsing headers: \n");

Pix temp = Request.first();	// request line
String a;
	Request.next(temp);	// skip it
	while (temp)
	{
		a = Request(temp);

		Debug(a);

		if (a.contains("Referer: ", 0))
			Referer = a.after(8);	// pos of ":"
		else
		if (a.contains("From: ", 0))
			From = a.after(5);	// pos of ":"
		else
		if (a.contains("User-Agent: "))
			UserAgent = a.after(11);	// pos of ":"
		else
		if (a.contains("Accept: "))
			Accept.append(a.after(7));
		else
			others.append(a);	// add the whole header to the 'others' list

		Request.next(temp);
	}


}


/* ParseRequestLine
 *	Break request line into Command, URL, and HTTP Version
 *
 * Russell Holt May 95
 * Moved from HTTP to httprequest July 27 1995
 *  - removed args & return value (because we're using class variables now)
 */
void httprequest::ParseRequestLine(void)
{
int url_index, http_vers_index;

	// the command, URL, and version are separated by whitespace
    url_index = Request_Line.index(" ");
    http_vers_index = Request_Line.index(" ", url_index + 1);

    Command = Request_Line.at(0,url_index);

	// outside of httprequest, use NewURL()
	URL = Request_Line.at(url_index+1, http_vers_index - url_index - 1);
	ParseURL();

    HTTP_Version = Request_Line.after(http_vers_index);

	if (HTTP_Version.contains("HTTP/1.0"))
		HTTP_Version = "HTTP/1.0";	// current
	else
		if (HTTP_Version.contains("HTTP/0.9"))
			HTTP_Version = "HTTP/0.9";	// old

#ifdef DEBUG
    cout << "\nCommand \"" << Command << "\" ";
    cout << "for the URL \"" << URL << "\" ";
    cout << "with HTTP version \"" << HTTP_Version << "\"\n";
#endif

	if (Command == "GET")
			cmd = CMD_GET;
		else
			if (Command == "POST")
				cmd = CMD_POST;
			else
				if (Command == "PUT")
					cmd = CMD_PUT;
				else
					if (Command == "DELETE")
						cmd = CMD_DELETE;
					else
						if (Command == "LINK")
							cmd = CMD_LINK;
						else
							if (Command == "UNLINK")
								cmd = CMD_UNLINK;
							else
								cmd = CMD_OTHER;
}



/*
 * Parse the URL!
 *
 * URL initially includes cgi data (if any).
 *
 * If there is cgi data, it gets truncated from the URL and put into
 * the String 'cgidata'.  The String 'Path' gets the actual path to
 * the "thing" - by prepending the document root to it.
 * Actual path parsing (ie "../" etc) is done in httpResHandler::GetURLInfo
 *
 * Russell Holt, May 3,10 1995
 * Moved from HTTP to httprequest July 27, 1995 (Russell)
 * variables now refer to httprequest class variables.
 */
void httprequest::ParseURL(void)
{
cgi CGIData;
String s = URL;

		// complete URL: "http://machine/url"
	if (URL.contains("http://", 0))
	{
		Debug("It is a full URL: machine ");
			// 7 is first char position after "http://"
		int m = URL.index("/", 7);	// next slash.
		String machine = URL.at(7, m-7);
		Debug(machine);

			// not really a URL, but that's what I'm calling it
		URL = URL.after(m-1);	// keep the /  ... ?
	}

	extra_path_pos = s.index("$");
	cgi_pos = s.index("?");
	show_request = 0;

	// Is this necessary?  Do some browsers f*ck with the URL like this?
	// s = CGIData.unescape_hex(URL);

#ifdef DEBUG
	cerr << "***\nextra_path_pos = " << extra_path_pos;
	cerr << ", cgi_pos = " << cgi_pos << '\n';
#endif

	doingCGI = (cgi_pos > 0);

	if (doingCGI)
	{
		cgidata = s.after(cgi_pos);
		if (extra_path_pos > 0)
		{
			URL = s.before(extra_path_pos);
			extra_path = s.at(extra_path_pos + 1, cgi_pos - extra_path_pos - 1);
			AddDocRoot(extra_path, extra_path_real);
		}
		else
			URL = s.at(0, cgi_pos);	// s.before(cgi_pos) ?

	}
	else
		if (URL[URL.length()-1] == '*')
		{
			if (extra_path_pos > 0)
				URL = s.before(extra_path_pos);
			else
				URL = s.at(0, s.length()-1);
			show_request = 1;
		}

#ifdef DEBUG
	if (extra_path_pos > 0)
		cout << "\nExtra path info is \"" << extra_path << "\"\n";

	if (cgi_pos > 0)
		cout << "\nCGI data is: \"" << cgidata << "\"\n";

	cout << "new URL is: \"" << URL << "\"\n";
#endif

	AddDocRoot(URL, Path);
}


void httprequest::AddDocRoot(String& old, String &New)
{
	if (old[0] == '/' && document_root[document_root.length()-1] == '/')
		New = document_root + old.after(0);
	else
		New = document_root + old;
}
