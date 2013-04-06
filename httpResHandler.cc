#include "httpResHandler.h"
#include "SLList_util.h"
#include "SLListSock.h"
#include "surl.h"
#include "R_Surlist.h"


extern "C" {
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/param.h>
char *realpath(const char *, char[MAXPATHLEN]);
void bcopy(const char *, char *, int);
int wait(int *);
int system(const char *);
int pipe(int fd[2]);
int dup2(int fd1, int fd2);
int fork(void);
int close(int);
}

#define LSCMD "ls -1a "

#define FILE_ICONS_DIR "file_icon_gifs"
#define FOLDER_ICON "medfolder.gif"
#define REG_FILE_ICON "medreg.gif"
#define HTML_FILE_ICON "medhtml.gif"
#define GIF_FILE_ICON "medgif.gif"
#define DIRECTORY_HEADER ".http_contents"
#define DEFAULT_INDEX "index.html"

#ifndef DESTINY_HTTP_ADDR
#define DESTINY_HTTP_ADDR "http://www.destinyusa.com/"
#endif

#define SKIP_DOT_FILES 1

#define MIME_HTML "text/html"
#define MIME_PLAIN "text/plain"

#define DIE(s) cout << s; exit(1)

/* The HTTP Resource Handler
 *
 *  Resource related things
 *
 *  History of changes
 *
 *	July 24-28: Major restructuring.
 *	- Incorporation of httprequest class -> removed many class variables
 *	- Removal of CGI parsing code, using cgi class instead.
 *	- ExternalCGI() opens pipe to child process and reads the data back
 *	  (looking for "Location: ..." on the first line)
 *  ...
 *
 *	June: MetaURL & SURLs, internal CGI & external CGI process launching...
 *  ...
 *
 *  May 18: 
 *
 *  May 17: working
 *	- moved more control to the methods (only GET for now)
 *  - wrote simple CGI program in the GET method
 *  - wrote DirectoryIndex
 *
 *  May 16: Created, unfinished.
 */

/*
 * Constructors
 */
httpResHandler::httpResHandler(void) : resHandler()
{
	Request = NULL;
	protocol = NULL;
	MimeInfo = NULL;
//	MetaURL = NULL;
	TheSurl = NULL;
	server = NULL;
}

httpResHandler::httpResHandler(httprequest *request, httpresponse *response,
	HTTP *a_protocol, Mime *mime, Socket *sock, httpserver *serv)
		: resHandler(sock)
{
	Request = request;
	Response = response;
	protocol = a_protocol;
	MimeInfo = mime;
//	MetaURL = MU;
	TheSurl = NULL;
	server = serv;
}

/*
 *  Get
 */
int httpResHandler::Get(void)
{
int done = 1;	// default to GET then be done.

	Debug("httpResHandler::Get()\n");

	// Special cases of SURL: metaURL, external CGI
	if (TheSurl)	// TheSurl was created in GetURLInfo()
	{
		int surltype = TheSurl->Type();
//
// ** "MetaURL" (surl directory) is disabled for now
// ** until I rewrite it with RSL in mind
//
//		if (surltype == type_META)
//		{
//			protocol->BeginDataTransfer();	// signal the start
//
//			*Sock << "<title>Main Directory</title>";
//			*Sock << "<h1>Main Directory</h1>";
//			MetaURL->Send(*Sock);
//			//	*Sock << *MetaURL;
//			*Sock << "<hr><p>";
//
//			//	protocol->BounceRequestResponse();
//			//	Mod_Message();
//
//			Destiny_Message();
//
//			protocol->EndDataTransfer();
//			return done;
//		}
//		else
			if (surltype == type_CGIExternal)
			{
				done = CGI();
				return done;
			}
			
			if (surltype = type_rsl)
			{
				
			}
	}

		// Can't call cgi programs that don't have cgi data unless they're
		// in the metaURL, because the only way to determine if a URL is
		// a cgi program is either a) it has CGI data appended to it, or
		// b) I say so (ie, it is a SURL).
		//
		// So what I'm saing is that this is the case where the URL
		// referred to a CGI program which was not a SURL.
	if (Request->cgi_pos > 0)
		done = CGI();
	else
	{
/*
		int testset = seteuid(65534);	// setuid(65534);

#ifdef DEBUG
		Debug("httpResHandler::Get()- setuid to nobody: ");
		if (testset == 0)
			Debug("succeeded.\n");
		else
			Debug("failed.\n");
#endif
*/

		if (Request->directory)
			DirectoryIndex();
		else
		{
			protocol->BeginDataTransfer();
			protocol->SendContent(Request->Path);
		}
		protocol->EndDataTransfer();
	}
	return done;
}

/***********************************************************************
 *  Find out stuff about the URL, actually, the pathname
 *  that resulted from parsing the URL (and prepending the
 *  document_root to it)
 *
 *  Return an HTTP status code (defined in http.h) describing
 *  the accessibility of the URL.
 */
int httpResHandler::GetURLInfo(String& cont_type,
	String& Modification_Date, String& cont_length, int& trans_mode)
{
	char *real_path = new char[2048];	// a big path
	realpath(Request->Path.chars(), real_path);
	String resolved_path = real_path;
	delete real_path;

#ifdef DEBUG
	cerr << "PATH: " << Request->Path << " ---> " << resolved_path << '\n';
#endif

	// here: check meta-url table
	// for now...

//	if ((TheSurl = (*MetaURL)(Request->URL)))
	if ((TheSurl = server->GetSurl(Request->URL)))
	{
		int surltype = TheSurl->Type();

#ifdef DEBUG
		cerr << "\n === The SURL is ";
		TheSurl->print();
		cerr << "  type " << surltype << " ===\n";
#endif
		// Check IP and Host authentication

		if ((TheSurl->totalRequiredIPs() && !TheSurl->MatchesIP(Sock->RemoteIP())) ||
			((TheSurl->totalRequiredHosts() && !TheSurl->MatchesHost(Sock->RemoteHost()))))
		{
			Response->logmessage = "<b>IP/Host Denied</b>";
			Debug(Response->logmessage);
			cont_type = MIME_HTML;
			trans_mode = MODE_ERROR;
			return STATUS_FORBIDDEN;
		}
#ifdef DEBUG
		else
			Debug(" IP/Host Accepted\n");
#endif

		String *sp = protocol->HTTPCurrentTimeDate();
		file_surl *ts;
		switch(surltype)
		{
			case type_BuiltIn:
				trans_mode = MODE_HTML;
				cont_type = MIME_HTML;
				Modification_Date = *sp;
				break;

			case type_CGIExternal:
				Modification_Date = *sp;
				sprintf(mod_date, "%s", sp->chars());
					// http::doingCGI originally set in http::ParseURL()
				Request->doingCGI = 1;

				// now don't break...

			case type_FILE:
				Request->Path = ((file_surl *) TheSurl)->File();
				break;

			case type_META:
				sprintf(mod_date, "%s", sp->chars());
				Modification_Date = *sp;
				cont_type = MIME_HTML;
				trans_mode = MODE_HTML;
				return STATUS_OK;
				break;

			case type_Directory:

				// Not allowed to specify directories outside of the
				// document root.
				Debug(" SURL type Directory: ");
				
				// if the requested url is the same as the SURL name
				// and the surl has an alt file then use the alt file
				// as the path.  eg,
				// if the directory is /russ and the requested url
				// is /russ, then I want a specific file returned instead
				// of a directory index (or the denial of one).
				// Added 10/2/95 -- RFH
				ts = (file_surl *) TheSurl;
				if ((ts->sFile()).length() > 0)
				{	// I hate f*cking with the slashes in directory names.
					String nm = ts->Url(), u = Request->URL;
					if ((u.reverse(),u)[0] == '/')	// a LISP technique
						u = reverse(u.after("/"));	// cut the trailing '/'
					if ((nm.reverse(),nm)[0] == '/')
						nm = reverse(nm.after("/"));
					if (u == nm)
						Request->Path = ts->File();
				}
				else
				if (!resolved_path.contains(Request->document_root, 0))
				{
					Debug("DENIED\n");
					cont_type = MIME_HTML;
					trans_mode = MODE_ERROR;
					return STATUS_FORBIDDEN;
				}
				Debug("ACCEPTED\n");
//				goto DirAltFile;	// alternate filename (in next case)
				break;

			case type_DirectoryLink:
				// allow links to things outside of the document root,
				// but *not* things whose owner gid < 10

				Debug(" SURL type DirectoryLink. ");
				// if the requested url is the same as the SURL name
				// and the surl has an alt file then use the alt file
				// as the path.  eg,
				// if the directory is /russ and the requested url
				// is /russ, then I want a specific file returned instead
				// of a directory index (or the denial of one).
				// Added 10/2/95 -- RFH
				ts = (file_surl *) TheSurl;
				if ((ts->sFile()).length() > 0)
				{	// I hate f*cking with the slashes in directory names.
					String nm = ts->Url(), u = Request->URL;
					if ((u.reverse(),u)[0] == '/')	// a LISP technique
						u = reverse(u.after("/"));	// cut the trailing '/'
					if ((nm.reverse(),nm)[0] == '/')
						nm = reverse(nm.after("/"));
					if (u == nm)
						Request->Path = ts->File();
				}
				else
					// only analyze if it is outside the doc root
				if (!resolved_path.contains(Request->document_root, 0))
				{
					Debug("Warning: path resolved outside document root- ");
					struct stat finfo;
					String fext;
					GetFileInfo(resolved_path, finfo, fext);
					uid_t ownid = finfo.st_uid;
					if (ownid < (uid_t) 10)	// Deny to group ID < 10
					{
						Debug("DENIED to owner group ID < 10\n");
						cont_type = MIME_HTML;
						trans_mode = MODE_ERROR;
						return STATUS_FORBIDDEN;
					}
					else
						Debug("ACCEPTED\n");
				}
				else
					Debug(" SURL directory ACCEPTED\n");
				
				break;
				
			case type_Location:
				Response->location = ((file_surl *)TheSurl)->File();
				cont_type = MIME_HTML;
				trans_mode = MODE_ERROR;
				return STATUS_MOVED;
				break;
			
			case type_rsl:
				cont_type = MIME_HTML;
				trans_mode = MODE_HTML;
				return STATUS_OK;
				break;
			default: ;
		}
	}
	else	// not a surl
		// if the document root is not a position 0 substring of the
		// fully resolved path, then don't send it!
		// Note: surls can override this, sort of.
		if (!resolved_path.contains(Request->document_root, 0))
		{
			Debug(" -- resolved path Denied!\n");
			trans_mode = MODE_ERROR;
			cont_type = MIME_HTML;
			return STATUS_NOT_FOUND;
		}
#ifdef DEBUG
		else
			Debug(" -- resolved path OK\n");
#endif


    // ------------------------------------------------------

//	// my first destiny server cgi programs
//	// *url is a String so == works
//	if (Request->URL == "/cgi-bin/test")
//	{
//		trans_mode = MODE_HTML;
//		return STATUS_OK;	// skip everything else
//	}
//



    // ------------------------------------------------------
    // find out file info

	struct stat file_info;
	String ext;

	int err = GetFileInfo(Request->Path, file_info, ext);

    if (err)
    {
        Debug("\nError: GetURLInfo: unable to get file information on",
			Request->Path);
        trans_mode = MODE_ERROR;
		cont_type = MIME_HTML;
        return STATUS_NOT_FOUND;
    }

    // ------------------------------------------------------
    // find the file type: how to send it.

mode_t file_mode = file_info.st_mode;

	/* 
	 * check file type
	 * should also check rwx permissions and user access list..
	 * 
	 */
				// regular file or sym link
	if (S_ISREG(file_mode) || S_ISLNK(file_mode))
	{
		Request->directory = 0;
		MimeInfo->GetType(ext, cont_type);


		// Some access control:
		// MURLonly == 1: send only SURLs
		// MURLonly == 2: send SURLs and images (mime type contains "image",
		// 		as in "image/gif")

		if (!TheSurl && server->SurlsOnly())	// SURL required
			if (server->SurlsOnly() == 1 || !cont_type.contains("image"))
			{
				// use an access log ?
				Response->logmessage = "<b>non-SURL denied</b>";
				Debug("Denied- SURL required\n");
				trans_mode = MODE_ERROR;
				cont_type = MIME_HTML;
				return STATUS_NOT_FOUND;	// forbidden?
			}

		if (Request->doingCGI)	// regular file & doing CGI... does extension matter?
		{
			trans_mode = MODE_HTML;

			// actually, CGI processes are required to send the
			// type themselves.  http::ProcessRequest doesn't send the
			// content type when doing CGI though.
			cont_type = MIME_HTML;

			// could check access here

			// need no modification date, no content-length
			// so leave now.
			return STATUS_OK;
		}

		Debug("Extension is ", ext);
		if (ext.length() <= 0)    // no extension
		{
			cont_type = MIME_PLAIN;
			trans_mode = MODE_ASCII;
		}
		else
		{
			trans_mode = MODE_BINARY;   // default

			// the only non-binary transfers

			if (cont_type.contains("htm"))
				trans_mode = MODE_HTML;
			else
				if (cont_type.contains("text"))
					trans_mode = MODE_ASCII;
		}
	}
	else
		if (S_ISDIR(file_mode))	// directory test
		{
			// check with the server to see if we want to
			// send directory listings
			if (!server->DirectoryIndexing())
			{
				Response->logmessage = "directory denied";
				cont_type = MIME_HTML;
				trans_mode = MODE_ERROR;
				return STATUS_FORBIDDEN;
			}

//			if (MURLonly && !TheSurl)
			if (server->SurlsOnly() && !TheSurl)
			{
				Response->logmessage = "non-SURL directory <b>denied</b>";
				Debug("Denied -- SURLs only\n");
				trans_mode = MODE_ERROR;
				cont_type = MIME_HTML;
				return STATUS_NOT_FOUND;	// forbidden?
			}
			Request->directory = 1;

			trans_mode = MODE_HTML;
			cont_type = MIME_HTML;
			// don't return here because directories can have
			// a modification date too (and that's next)
		}
		else
			{
				// can only handle regular files, sym links,
				// and directories.

				Response->logmessage = "<b>denied</b>";
				Debug("Denied -- not a regular file, sym link, or directory\n");
				trans_mode = MODE_ERROR;
				cont_type = MIME_HTML;	// for error messasge
				return STATUS_FORBIDDEN;
			}

    // ------------------------------------------------------
    // find last modification date.. (are gmtime(), strftime() portable?)

    time_t mod = file_info.st_mtime;    // the # seconds since jan 1 1970

    int r = protocol->DateFormat(&mod, mod_date, 79);

    Modification_Date = mod_date;

    // ------------------------------------------------------
    // extract file length
	long len = (long) file_info.st_size;

	if (len >= 0)
		cont_length = dec(len);
#ifdef DEBUG
	else
		Debug("GetURLInfo: file length < 0 ?? (ignored) \n");
#endif

    return STATUS_OK;
}


/* DirectoryIndex
 *  Create a hyperlinked directory index 
 *  1. do "ls -1" (one column) on the document_root/url
 *     into a temp file named `temp' appended with the current process ID.
 *  2. put each line of this file into a String in a StringSLList.
 *  3. send the directory header file, if it exists.
 *  4. for each filename, send this over the socket:
 *     <a href="url/filename">filename</a>
 *		since the url refers the the "current" directory.
 *		note that this is missing the document root because it
 *		gets prepended to requested URLs.
 *
 * Russell Holt, May 17 1995
 */
void httpResHandler::DirectoryIndex(void)
{
Socket& s = *Sock;
String tempfn = "/tmp/temp" + String("Destiny") + String(dec(getpid()));
String lscmd = String(LSCMD) + Request->Path + String(" > ") + tempfn;
//	String abspath = Path + tempfn;


	if (system(lscmd.chars()) == 127)	// "shell could not be executed" error
	{
		Response->status_code = STATUS_INTERNAL_ERROR;
		Response->CreateResponseLine(Request->HTTP_Version);
		protocol->HTTPStatusMessage();
		return;
	}

	protocol->BeginDataTransfer();

	// check for the existence of the default index html file

	s << "<title>Directory of " << Request->URL << "</title>";
	s << "<h1>Directory of " << Request->URL << "</h1><hr>";

	SLList<String> thefiles;
	ifstream in(tempfn.chars());
	if (!in)
		return;
	
	in >> thefiles;	// op defined in string_utils.h

	s << "<pre>";
//	s << thefiles;

Pix temp = thefiles.first();
String a;
struct stat file_info;
int err;
off_t filesize;
long len;
mode_t file_mode;
String mime_type, ext, real_file;
int ext_ind = -1;
String& url = Request->URL;	// shorthand
String& docroot = Request->document_root;

	// should add link to parent directory

	while (temp)
	{
		// for each string (filename) "abc" make this:
		// <a href="url/abc">abc</a>
		// where url prepended with document_root is the
		// absolute pathname of the directory whose filenames
		// are listed in the StringSLList

		a = thefiles(temp);

		real_file = docroot;
		if (docroot[docroot.length()-1] != '/'
				&& url[0] != '/')
			real_file += "/";
		real_file += url;
		if (url[url.length()-1] != '/' && a[0] != '/')
			real_file += "/";
		real_file += a;

			// send the directory header file
			// actually, it should be sent before anything else is
		if (a == DIRECTORY_HEADER)
		{
			s.SendAsciiFile(real_file);
			goto NEXT_ITER;
		}
		if (a[0] == '.' && SKIP_DOT_FILES)
			goto NEXT_ITER;

		err = stat(real_file.chars(), &file_info);    // how portable is this?
		len = (long) file_info.st_size;
		file_mode = file_info.st_mode;

		ext_ind = a.index(".", -1);

		if (ext_ind > 0)
			ext = a.after(ext_ind);

		s << "<img align=bottom src=\"" << DESTINY_HTTP_ADDR
			<< FILE_ICONS_DIR << "/";

			if (S_ISREG(file_mode) || S_ISLNK(file_mode))
			{
				if (ext_ind > 0)
				{
					MimeInfo->GetType(ext, mime_type);
					if (mime_type.contains("html"))
						s << HTML_FILE_ICON;
					else
						if (mime_type.contains("gif"))
							s << GIF_FILE_ICON;
						else
							s << REG_FILE_ICON;
				}
				else
					s << REG_FILE_ICON;
			}
			else
				if (S_ISDIR(file_mode))
					s << FOLDER_ICON;

		// should construct the string first, then send it..

		s << "\"> ";

		s << "<a href=\"" << url;

		// insert a `/` if necessary
		if (url[url.length() - 1] != '/' && a[0] != '/')
			s << "/";

		s << a << "\">" << a << "</a><br>\n";

NEXT_ITER:
		thefiles.next(temp);
	}
	s << "</pre>";

	if (Response->add_mod_date)
	{
		Mod_Message();
		Destiny_Message();
	}
	if (Request->show_request)
		protocol->BounceRequestResponse();

	unlink(tempfn.chars());	// delete temporary file
}


/* launch an external CGI process
 *
 * Russell Holt, June 5 1995
 * No CERN or NCSA code here (I didn't peek, either)
 */
int httpResHandler::ExternalCGI(void)
{
String ext;
struct stat file_info;
int err;
int done = 1;

	Response->logmessage = "CGI ";
	Debug("ExternalCGI: File ", Request->Path);

	err = GetFileInfo(Request->Path, file_info, ext);

	if (err)	// this should be caught in GetURLInfo() ...
	{
		Debug("   -- Does not exist.\n");
		Response->status_code = STATUS_INTERNAL_ERROR;
		Response->CreateResponseLine(Request->HTTP_Version);
		protocol->BeginDataTransfer();
		protocol->HTTPStatusMessage();
		*Sock << "<hr>CGI data is: <p><tt>";
		*Sock << Request->cgidata << "</tt><p>";
		return done;
	}

	String ip = Sock->RemoteIP();
	String rhost = Sock->RemoteHost();

	Debug("  -- Exists\n");

		char *envp[20];		// 19 environment variables.  Must end with NULL
		char all[4] ={ '*', '/', '*', 0 };	// http accept list (a fake)
		String s;

		// look in httpResHandler from version 0.4 to see another way
		// this was done (commented out there though):

		envp[0] = "SERVER_SOFTWARE=Destiny/0.04";
		envp[1] = "SERVER_NAME=unknown_host";
		envp[2] = "GATEWAY_INTERFACE=CGI/1.1";
		envp[3] = "SERVER_PROTOCOL=HTTP/1.0";

		s = String("SERVER_PORT=") + dec(Sock->Port());
		envp[4] = new char[80];
		sprintf(envp[4], "SERVER_PORT=%d", Sock->Port());

		envp[5] = new char[80];
		sprintf(envp[5], "REQUEST_METHOD=%s", Request->Command.chars());

		s = String("QUERY_STRING=") + Request->cgidata;
		envp[6] = new char[s.length() + 1];
		sprintf(envp[6], "%s", s.chars());

		envp[7] = new char[80];
			sprintf(envp[7], "SCRIPT_NAME=%s", Request->Path.chars());
		envp[8] = new char[80];
			sprintf(envp[8], "REMOTE_HOST=%s", rhost.chars());
		envp[9] = new char[80];
			sprintf(envp[9], "REMOTE_ADDR=%s", ip.chars());

		envp[10] = "AUTH_TYPE=no_auth";
		envp[11] = "REMOTE_USER=no_user";
		envp[12] = "CONTENT_LENGTH=no_length";

		envp[13] = new char[80];
		sprintf(envp[13], "HTTP_ACCEPT=a_list_here_later");

		String syzzyggy = String("HTTP_USER_AGENT=") + Request->UserAgent;
		envp[14] = new char[syzzyggy.length() + 1];
			sprintf(envp[14], "%s", syzzyggy.chars());

		syzzyggy = String("PATH_INFO=") + Request->extra_path;
		envp[15] = new char[syzzyggy.length() + 1];
			sprintf(envp[15], "%s", syzzyggy.chars());

		syzzyggy = String("PATH_TRANSLATED=") + Request->extra_path_real;
		envp[16] = new char[syzzyggy.length() + 1];
			sprintf(envp[16], "%s", syzzyggy.chars());

		envp[17] = NULL;

/*
		envp[17] = "REMOTE_IDENT=RFC931";	// RFC 931 id
		envp[18] = "CONTENT_TYPE=ForPostOrPutOnly";	// POST, PUT
		envp[19] = NULL;
*/

		// for now, the envp[] is ignored
		// this should be changed to execve
		// also need to add command-line for the ISINDEX query

		//	execl(Path.chars(), Path.chars(), NULL);

		char *argv[2];
		char *pn = new char[Request->Path.length()+1];
		bcopy(Request->Path.chars(), pn, Request->Path.length());

		argv[0] = pn;
		argv[1] = NULL;	// only one c-line arg for now- process name

	int rwpipe[2];
	if (pipe(rwpipe) < 0)
	{
		perror("httpResHandler::ExternalCGI()\n");
		return 1;
	}

//-----------

	gid_t cgi_uid = file_info.st_uid;

//-----------

	int cpid;
	if ((cpid = fork()) == 0)	// child
	{
		close(rwpipe[0]);	// read end of pipe

		dup2(rwpipe[1], 1);	// stdout is now write end of pipe

		// set uid to be that of the owner of the cgi program
		int gidtest = setgid(cgi_uid);

#ifdef DEBUG
		Debug("httpResHandler::ExternalCGI()- setuid to ", cgi_uid);
		if (!gidtest)
			Debug("succeeded.\n");
		else
			Debug("failed.\n");
#endif
		execve(Request->Path.chars(), argv, envp);
	}
	else	// parent
	{
		close(rwpipe[1]);	// write end of pipe

			
		String firstline;	//	first line read from parent
							//		(looking for "Location: ...")
		char buf[512];		//	large enough to contain a really
							//		huge "Location: ..." URL
		int size=0, first = 1, totalbytes=0;

		// Read first line
		size=read(rwpipe[0], buf, 511);

		Debug("CGI--- Read ", size, " bytes from pipe\n");
		
		firstline = buf;
		if (firstline.contains("Local: ", 0))
		{
			Response->logmessage += "local-redirect";
			Debug("FIRSTLINE: ", firstline);

			// strip "Local: "
			//	String newurl = firstline.after(firstline.index(":") + 1);
			String newurl = firstline.after(RXwhite);

			// strip everything after the first whitespace
			//	int newline = newurl.index(RXwhite);
			//	newurl = newurl.before(newline);
			newurl = newurl.before(RXwhite);

			Debug("CGI Parent: got URL ", newurl);

			// Need to GET a different URL, so we can't
			// begindatatransfer yet!!
			Request->NewURL(newurl);	// substitute URL and re-parse
			done = 0;	// tell everyone else that we're not done!

			goto ParentDoneReading;
		}
		else
			if (firstline.contains("Location: ", 0))
			{
				// Log the foreign redirect, and make a link to it
				// in the log file !! ha-ha!!
				String newurl = firstline.after(RXwhite);
				newurl = newurl.before(RXwhite);
				Response->logmessage = String("<a href=\"") +
					newurl + String("\">foreign-redirect</a>");

				// Tell the browser that it is in a different location
				Response->status_code = STATUS_FOUND;
				Response->CreateResponseLine(Request->HTTP_Version);

				// the "Location: " string becomes a response header
				protocol->BeginDataTransfer();
				Sock->writen(buf, size);
			}
		else
		{
			protocol->BeginDataTransfer();	// IMPORTANT!!!
			// write the first line from the child to the socket.
			Sock->writen(buf, size);
		}



		// read from child, write to socket.
		while (1)
		{
			size=read(rwpipe[0], buf, 511);
			Debug("CGI--- Read ", size, " bytes from pipe\n");

			if (size<=0)
				break;

				//	write the data from the child to the socket.
				// Here is where we would translate it (say, for
				// encryption or snooping)  or, maybe just add
				// it to a larger buffer to send later (trouble)
			Sock->writen(buf, size);
			totalbytes += size;
		}

		// Should write out totalbytes to the log file here
		// (before ParentDoneReading - "Location: ..." doesn't transfer
		// any bytes.

ParentDoneReading:

		close(rwpipe[0]);	// close the pipe
		Debug("\n=== Waiting on pid ", cpid, "\n");

		int childstat;
		int childstatpid = wait(&childstat);
		Debug("\n=== Done\n");
	}

	return done;
}

/* CGI
 *  This is the main CGI router
 *
 * Russell Holt, May/June 1995
 */
int httpResHandler::CGI(void)
{
int done = 1;

	// IMPORTANT! DO NOT BeginDataTransfer() for ExternalCGI yet!!!
	done = ExternalCGI();
	protocol->EndDataTransfer();

	return done;
}


void httpResHandler::Destiny_Message(void)
{
	String s = Request->UserAgent;
	s = s.before((int)(s.length()-2));
	*Sock << "<p>Brought to you <i>"
		<< s;
	*Sock << "</i> by the <a href=\"http://www.destinyusa.com/\">Destiny</a> Server<p>";
}

/*
httpResHandler::
*/
