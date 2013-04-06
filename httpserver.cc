// NAME: httpserver.cc
//
// PURPOSE:
//
// PROGRAMMING NOTES:
//
// HISTORY: 
//
// $Id: httpserver.cc,v 1.4 1995/10/26 18:20:14 holtrf Exp holtrf $
//
// Copyright 1995 by Destiny Software Corporation.


#include "httpserver.h"
#include "../crb/R_html.h"
#include "SLList_res_util.h"

#include <stream.h>

void httpserver::Init(void)
{
	cli_mode = 0;	// obsolete
//	block_io = 1;	// obsolete
	http = new HTTP();
//	metaurlOnly = 0;
//	directoryIndexing = 0;

//	host_name = "www.destinyusa.com";
//	rererer_log = "/www/htdocs/dest/referers.html"

//	document_root = "/";
//	accessLogName = "/mongo/dest/server/access_log.html";
	
	SetupServerVars();
}

// SetupServerVars
// Makes the global restable to use in scripting,
// and adds server resource variables.
void httpserver::SetupServerVars(void)
{
	globals = new restable("Globals");
	globals->MakeExist(exist_perm);
	globals->install_resource(globals);

	scripter.SetGlobalTable(globals);
	locals = new restable("locals");

	Port.MakeExist(exist_perm);
	globals->Add(&Port);	// from Server.h

	surls_only.MakeExist(exist_perm);
	globals->Add(&surls_only);

	directoryIndexing.MakeExist(exist_perm);
	globals->Add(&directoryIndexing);

	document_root.MakeExist(exist_perm);
	globals->Add(&document_root);

	accessLogName.MakeExist(exist_perm);
	globals->Add(&accessLogName);

	host_name.MakeExist(exist_perm);
	globals->Add(&host_name);	// from Server.h

	referer_log.MakeExist(exist_perm);
	globals->Add(&referer_log);	// from Server.h

	server_address.MakeExist(exist_perm);
	globals->Add(&server_address);

	webmaster_email.MakeExist(exist_perm);
	globals->Add(&webmaster_email);

	printordered(globals->GetList());

	// Add the surl list
	// don't add this as a reference object, because there is
	// only one of these.
	surlist = new R_Surlist("Surls");
	surlist->MakeExist(exist_perm);
	globals->Add(surlist);

	// add the HTML type
	R_html *rhtml = new R_html;
	rhtml->MakeExist(exist_perm);
	scripter.AddReferenceObject(rhtml);

	// before we execute the startup script, we must make a few things
	// safe by disabling a few key resources so that arbitrary rsl scripts
	// cannot be run as root -- if by some chance a hacker were able to
	// have the server read his scripts at startup.
//	resource *r = scripter.GetResource("File");
//	if (r) r->SetEnable(0);	// no reading/writing of external files from RSL.

}

void httpserver::CommandLine(int argc, char **argv)
{
int c;
GetOpt getopt(argc, argv, "m:bp:d:hl:ys:r:f:");

    while ((c = getopt()) != EOF)
        switch(c)
        {
			case 'l':	// logfilename
				accessLogName = String(getopt.optarg);
				break;
			case 'm':
				surls_only = atoi(getopt.optarg);
				break;
            case 'p':
                Port = atoi(getopt.optarg);
                break;
            case 'd':
                document_root = String(getopt.optarg);
                break;
//			case 'b':
//				block_io = 1;
//				break;
			case 'y':
				directoryIndexing = 1;
				break;
			case 'f':
				scripter.ParseFile(getopt.optarg);
				break;
            case 'h':
            case '?':
                cout << "Usage: " << argv[0]
					 << " [-p port] [-d document-root]"	// [-b]"
					 << " [-m accept-level] [-l access-log] [-y]"
					 << " [-s server host name] [-r referer logfile]"
					 << " [-f config script]\n\n";
				cout << "p: port to listen on\n";
				cout << "d: the directory that the URL \"/\" maps to\n";
//				cout << "b: I/O blocking when listening for connections\n";
				cout << "m: 1 = send only SURLs,\n"
					 << "   2 = send only SURLs and files of MIME type"
					 << " \"image\"\n";
				cout << "l: path to the access log file\n";
				cout << "y: allow directories to be indexed and shown\n\n";
				cout << "s: name this host is addressed by on the WWW\n";
				cout << "r: logfile for foreign referers (.html)\n";
				cout << "f: RSL configuration script path\n";
                _exit(0);
       		case 's':	host_name = String(getopt.optarg);
       			break;
       		case 'r':	referer_log = String(getopt.optarg);
       			break;
            default: ;
        }
}

/*
 *  Setup http specific stuff.
 * Russell Holt, May 9 1995
 *
 * ModifiedMay 1995 - MIME info added
 * Modified in June 1995 for metaURL
 * Modified July 6, 1995 for access log (should be moved up to Server)
 */
void httpserver::Setup(void)
{
#ifdef DEBUG
	cout << "httpserver::Setup(): executing startup script\n";
	cout.flush();
#endif
	//	scripter.execute("Main", locals);	// run startup script

	if (globals)
	{
		resource *runner = globals->lookup("Main");
		if (runner && runner->ClassName() == "List")
			((restable *) runner)->execute(NULL, globals, NULL, locals);
		else
		{
			cerr << "No `Main' ???\n\n";
			exit(1);
		}
	}
	else
	{
		cerr << "..arg, no global table ....\n";
		exit(1);
	}

/*
//	String xyz = String("<hr><b>Server started up ") + Now();
//	xyz += "</b><br>\n";
//	Log(xyz.chars());
	cout << "logging..";
	String x = Now();
	//	logfile.Openlog(accessLogName.chars());
	logf = new slog;
	logf->Openlog("/mongo/dest/server/acc_log", _to_stdio_file_);
	logf->Log(1, "<hr><b>Server started up %S </b><br>\n", &x);
*/
	TheMime = new Mime("mime.conf");
    TheMime->ReadConfigFile();

//	createTestMetaURL();
#ifdef DEBUG
    cout << "Listening on port " << Port << '\n';
	cout << "Access log is " << accessLogName << '\n';
    cout << "Document root is " << document_root << "\n\n";
    cout.flush();
#endif

}

/* NewConnection
 *  What to do when a new connection comes in.
 */
void httpserver::NewConnection(void)
{
#ifdef DEBUG
	cerr << "Server Child process: " << getpid() << "\n";
#endif

	ListenSock->Close();
	delete ListenSock;

	http->Init(ClientSock, document_root.Value(), TheMime, this);
	http->Handle_Request();
//	_exit(0);
}

void httpserver::Run(void)
{

//	if (block_io)
		ListenLoop();
//	else
//		NonBlockListenLoop();
}


/*  Testing the MetaURL
 *
 *  Create a metaURL and add a bunch of SURLs.
 *  Most are for CGI processes and are thus invisible.
 *
 * Russell Holt June 2 1995 & later
 */
/*
void httpserver::createTestMetaURL(void)
{
surl *a;
String name, file, title;
int x;

#ifdef DEBUG
	cout << "\nCreating the metaURL...\n";
#endif

	name = "/metaURL";
	TheMetaURL = new metaurl(name);


	name = "/";					// name of SURL alias
	file = "/www/htdocs/dest/destiny.html";			// path of real file
	title = "Destiny Software Corporation";	// title
	a = new file_surl(name, file, title);
	a->SetVisibility(1);
	a->SetActive(1);
	TheMetaURL->add(a);

	// The directory of the destiny homepage
	// allows links outside the doc root
	// "index" file is destiny.html
	name = "/dest";
	a = new file_surl(name, "/www/htdocs/dest/destiny.html", type_DirectoryLink);
	a->SetVisibility(0);
	TheMetaURL->add(a);

	name = "/cgi-bin/imagemap";
	file = "/disk2/httpd_1.3/cgi-bin/imagemap";
	a = new file_surl(name, file, type_CGIExternal);
	a->SetVisibility(0);
	TheMetaURL->add(a);

	name = "/cgi-bin/dimagemap";
	file = "/disk2/httpd_1.3/cgi-bin/dimagemap";
	a = new file_surl(name, file, type_CGIExternal);
	a->SetVisibility(0);
	TheMetaURL->add(a);

	// A redirect url
	name = "/netstatus";
	file = "http://www.destinyusa.com/netstatus/";	// not really a file - a url hack
	a = new file_surl(name, file, type_Location);		// everything in the directory
	a->SetVisibility(0);
	TheMetaURL->add(a);
	
	// A directory
	name = "/netstatus/";
	file = "/www/htdocs/netstatus/usa.html";
	a = new file_surl(name, file, type_Directory);		// everything in the directory
	a->SetVisibility(0);
	TheMetaURL->add(a);

	name = "/russ";
	file = "http://www.destinyusa.com/russ/";
	a = new file_surl(name, file, type_Location);
	a->SetVisibility(0);
	TheMetaURL->add(a);

	// A directory which allows links outside of the document root
	name = "/russ/";
	file = "/www/htdocs/russ/russ.html";
	a = new file_surl(name, file, type_DirectoryLink);	// everything in the directory
	a->SetVisibility(0);
	TheMetaURL->add(a);

	name = "/rsl";
	file = "http://www.destinyusa.com/rsl/";
	a = new file_surl(name, file, type_Location);
	a->SetVisibility(0);
	TheMetaURL->add(a);

	name = "/rsl/";
	file = "/www/htdocs/rsl/rsl.html";
	a = new file_surl(name, file, type_DirectoryLink);	// everything in the directory
	a->SetVisibility(0);
	TheMetaURL->add(a);

	name = "/rsl/source";
	file = "/www/htdocs/rsl/x.html";
	title = "rsl source index";
	a = new file_surl(name, file, title);
	a->AddRequiredHost(".destinyusa.com");
	a->SetVisibility(1);
	a->SetActive(1);
	TheMetaURL->add(a);

	name = "/accesslog";	// alias
	file = "/mongo/dest/server/acc_log.txt";
	title = "Destiny server access log (destinyusa.com only)";
	a = new file_surl(name, file, title);
	a->AddRequiredHost(".destinyusa.com");
	a->SetVisibility(1);
	a->SetActive(1);
	TheMetaURL->add(a);
	
	name = "/cgi-bin/cgirtest";
	file = "/home/holtrf/dis/sock/5/cgirtest";
	a = new file_surl(name, file, type_CGIExternal);
	a->SetVisibility(0);
	TheMetaURL->add(a);

	name = "/cgi-bin/seditor";
	file = "/mongo/dest/bin/seditor";
	a = new file_surl(name, file, type_CGIExternal);
	a->SetVisibility(0);
	TheMetaURL->add(a);

	// an external cgi surl that takes no CGI data
	name = "/cgi-bin/date";		// alias
	file = "/home/holtrf/dis/sock/4/date_test";	// real script
	a = new file_surl(name, file, type_CGIExternal);
	a->SetVisibility(0);
	TheMetaURL->add(a);

	// an external cgi surl- with NCSA CGI code
	name = "/cgi-bin/ncsa-test";		// alias
	file = "/home/holtrf/dis/sock/4/cgi/test";
	a = new file_surl(name, file, type_CGIExternal);
	a->SetVisibility(0);
	TheMetaURL->add(a);

	// an external cgi surl that takes no CGI data
	// shows the contents of the bboard
	name = "/showboard";		// alias
	file = "/home/holtrf/dis/sock/4/cgi/showboard";	// real script
	title = "The Board";
	a = new file_surl(name, file, title);
	a->SetType(type_CGIExternal);
	//	a->RequireIP("204.120.87.");
	TheMetaURL->add(a);

	// an external cgi surl- with NCSA CGI code
	// adds comments to the bboard
	name = "/adder";		// alias
	file = "/home/holtrf/dis/sock/4/cgi/adder";	// real executable
	a = new file_surl(name, file, type_CGIExternal);
	a->SetVisibility(0);
	TheMetaURL->add(a);

	// form to add comments - uses surl "/adder"
	name = "/add-comments";					// name of SURL alias
	file = "/www/htdocs/adder.html";		// name of real file
	title = "Add your comments to The Board";	// title to display when printed
	a = new file_surl(name, file, title);
	TheMetaURL->add(a);

	// an external cgi surl that takes no CGI data
	// shows the contents of the status map
	name = "/cgi-bin/status-map";		// alias
	file = "/home/holtrf/dis/sock/4/cgi/status";	// real script
	a = new file_surl(name, file, type_CGIExternal);
	a->SetVisibility(0);
	TheMetaURL->add(a);

	// an HTML form interface to the status mapper CGI program (above).
	name = "/status";		// alias
	file = "/www/htdocs/status-map2.html";	// real file
	title = "Status Map";
	a = new file_surl(name, file, title);
	a->SetVisibility(0);
	TheMetaURL->add(a);

	// an external cgi surl
	// reads a status-map file and produces a form to modify all
	// the data in it (but not to add or delete).. This form sends
	// output to /cgi-bin/status3 (below)
	name = "/cgi-bin/status2";		// alias
	file = "/home/holtrf/dis/sock/4/cgi/status2";	// real program
	a = new file_surl(name, file, type_CGIExternal);
	a->SetVisibility(0);
	TheMetaURL->add(a);

	// an external cgi surl
	// Produces 'sed' commands to modify the status-map file;
	// called from the form created by /cgi-bin/status2
	name = "/cgi-bin/status3";		// alias
	file = "/home/holtrf/dis/sock/4/cgi/status3";	// real program
	a = new file_surl(name, file, type_CGIExternal);
	a->SetVisibility(0);
	TheMetaURL->add(a);

	// an HTML form status map editor
	name = "/status-editor";		// alias
	file = "/www/htdocs/status/status-editor.html";	// real file
	title = "Status Map Editor";
	a = new file_surl(name, file, title);
	a->SetVisibility(0);
	TheMetaURL->add(a);


	// A file surl which requires a particular IP address
	// will be in the directory table only for this IP
	name = "/iptest";	// alias
	file = "/www/htdocs/ddes.html";	// real file
	title = "TEST - 204.120.87.12 only";
	a = new file_surl(name, file, title);
	a->AddRequiredIP("204.120.87.12");
	TheMetaURL->add(a);

	// Another file surl which requires an IP address domain
	// will be in the directory table only for this domain
	name = "/domaintest";	// alias
	file = "/www/htdocs/ddes.html";	// real file
	title = "TEST - 204.120.87.* ONLY";
	a = new file_surl(name, file, title);
	a->AddRequiredIP("204.120.87.");
	TheMetaURL->add(a);

	// file surl which requires a host
	// will be in the directory table only for this host
	name = "/hosttest";	// alias
	file = "/www/htdocs/ddes.html";	// real file
	title = "TEST - ranger.destinyusa.com ONLY";
	a = new file_surl(name, file, title);
	a->AddRequiredHost("ranger.destinyusa.com");
	TheMetaURL->add(a);

	// file surl which requires a host domain
	// will be in the directory table only for this domain
	name = "/host-domain-test";	// alias
	file = "/www/htdocs/ddes.html";	// real file
	title = "TEST - *.destinyusa.com ONLY";
	a = new file_surl(name, file, title);
	a->AddRequiredHost(".destinyusa.com");
	TheMetaURL->add(a);

	// file surl which requires a host domain
	// will be in the directory table only for this domain
	name = "/ahern/this";	// alias
	file = "/home/holtrf/cstp/cstp-server_8-16-95.eps";	// real file
	title = "Only sphere.llnl.gov";
	a = new file_surl(name, file, title);
	a->AddRequiredHost("sphere.llnl.gov");
	TheMetaURL->add(a);

	// file surl which requires a host domain
	// will be in the directory table only for this domain
	name = "/host-domain-test";	// alias
	file = "/www/htdocs/ddes.html";	// real file
	title = "TEST - ranger, raja ONLY";
	a = new file_surl(name, file, title);
	a->AddRequiredHost("ranger.destinyusa.com");
	a->AddRequiredHost("raja.destinyusa.com");
	TheMetaURL->add(a);

	// file surl which requires a host domain
	// will be in the directory table only for this domain
	name = "/host-domain-test";	// alias
	file = "/www/htdocs/ddes.html";	// real file
	title = "TEST - 204.120.87.10 and 204.120.87.12 ONLY";
	a = new file_surl(name, file, title);
	a->AddRequiredIP("204.120.87.10");
	a->AddRequiredIP("204.120.87.12");
	TheMetaURL->add(a);

	name = "/shelltesteralias";			// name of SURL alias
	file = "/www/htdocs/testshell";		// path of real shell script
	a = new file_surl(name, file);
	a->SetVisibility(0);				// invisible in the metaURL
	TheMetaURL->add(a);

	// The code for this surl is compiled in the server
	a = new surl("/cgi-bin/test", type_BuiltIn);
	a->SetVisibility(0);
	TheMetaURL->add(a);



#ifdef DEBUG
	TheMetaURL->print();
#endif
}

*/

// Now
// - returns an ascii string with the current time.
char *httpserver::Now(void)
{
	time_t now = time(0);
	struct tm *ltime = localtime(&now);
	return asctime(ltime);
}
