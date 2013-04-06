#ifndef _HTTP_SERVER_H_
#define _HTTP_SERVER_H_ 1

#include "unixsystem.h"
#include "Server.h"
#include <stream.h>
#include <GetOpt.h>
#include <String.h>
#include "Socket.h"
#include "http.h"
#include "Mime.h"
//	#include "metaurl.h"
#include <stdio.h>
#include "../crb/rsl_control.h"
#include "../crb/restable.h"
#include "../crb/R_String.h"
#include "../crb/R_Boolean.h"
#include "../crb/R_Integer.h"
#include "R_Surlist.h"

class HTTP;

//	class Mime;

class httpserver : public Server {
	void Init(void);
protected:
	int cli_mode;	// obsolete
	
//	String document_root, accessLogName;
	R_String document_root, accessLogName;

	Mime *TheMime;
	HTTP *http;
	
//	int metaurlOnly;
//	int directoryIndexing;
	
	R_Boolean surls_only;
	R_Boolean directoryIndexing;

	R_String server_address, webmaster_email;

	rsl_control scripter;
	restable *globals, *locals;
	R_Surlist *surlist;

	void SetupServerVars(void);
	
public:
	httpserver(void) :document_root("DocumentRoot", ""),
		accessLogName("AccessLog", ""),
		surls_only("SurlsOnly", 0),
		directoryIndexing("DirectoryIndexing", 0),
		server_address("ServerAddress", ""),
		webmaster_email("WebMasterEmail", "")
	{ Init(); }

	void CommandLine(int, char **);
	void NewConnection(void);
	void Setup(void);
	void Run(void);
//	void createTestMetaURL(void);

	inline R_Surlist *GetSurlist(void) { return surlist; }
	inline surl *GetSurl(String su) { return surlist?(*surlist)(su):NULL; }
	inline restable *GetGlobals(void) { return globals; }
	inline rsl_control& Scripter(void) { return scripter; }

	inline int SurlsOnly(void) { return surls_only.LogicalValue(); }
	inline int DirectoryIndexing(void) { return directoryIndexing.LogicalValue(); }
	inline String DocumentRoot(void) { return document_root.Value(); }
	inline String AccessLogName(void) { return accessLogName.Value(); }
	inline String ServerAddress(void) { return server_address.Value(); }
	inline String WebMasterEmail(void) { return webmaster_email.Value(); }

	char *Now(void);

};

#endif
