// NAME: surl.cc
//
// PURPOSE:
//
// PROGRAMMING NOTES:
//
// HISTORY: 
//
// $Id$

#include "surl.h"

const static char *TheTypes[MaxTypes] = {
	"(No type)",	// 0
	"Built-in",
	"File",
	"CGI/Int",
	"CGI/Ext",
	"Fwd-Req",
	"Special",
	"Directory",	// 7
	"(undefined)",	// 8
	"(undefined)",
	"(undefined)",
	"META",			// 11
	"Location",
	"internal RSL"
};

surl::Init(void)
{
	url="no url";
	type = 0;
	visible = 1;
	active = 1;
	show_modified_date = 1;
//	n_IPs = n_hosts = n_users = 0;	// obsolete - use list length instead
	dynamic = NULL;
}

surl::surl(void)
{
	Init();
}

//--- url, type -------
surl::surl(String& n, int t)
{
	Init();
	url=n;
	type=t;
}

//--- url, type -------
surl::surl(char *n, int t)
{
	Init();
	url=n;
	type=t;
}

//--- url, type, title -------
surl::surl(String& n, int t, String& tt)
{
	Init();
	url=n;
	type=t;
	title = tt;
}

//--- url, type, title -------
surl::surl(char *n, int t, char *tt)
{
	Init();
	url=n;
	type=t;
	title = tt;
}

surl::~surl(void)
{
}

int surl::MatchesUrl(String a_url)
{
	return (a_url == url);
}

/* _MatchesHost (protected)
 * 
 * Returns true (1) if a given hostname is acceptable to this SURL.
 * That is, the given hostname is in the list of acceptable hosts.
 * Otherwise, return 0.  Interfaced through MatchesHost()
 *
 * Example,
 * If the required host is ".com", and the remote host is "a.b.com",
 *
 * 1) reverse both strings
 * 2) check if required host is a substring of the remote host from
 *    position 0, ie if "moc." is a substring of "moc.b.a" (which is
 *    easier than doing it from the end of the string).
 *
 * This method allows one to specify something as general as ".com"
 * or as specific as "ranger.destinyusa.com"
 *
 * Russell Holt early July 1995
 * modified July 20, 1995 for StringSLList - arbitrary # of hosts
 *  -- perhaps this should be a preset size rather than so
 *     arbitrarily large?  An array of char * rather than a linked
 *     list of strings..?  The list method is of course more flexible
 *     but every little performance hit counts...
 */
int surl::_MatchesHost(char *ho)
{
Pix temp = requireHost.first();
String host = reverse(ho), reqhost;

#ifdef DEBUG
	cerr << "_MatchesHost() -- n_hosts = " << requireHost.length() << '\n';
#endif

	while(temp)
	{
		reqhost = reverse(requireHost(temp));

		if (host.contains(reqhost, 0))
		{
#ifdef DEBUG
			cerr << "Host Accepted\n";
#endif
			return 1;
		}
		requireHost.next(temp);
	}

#ifdef DEBUG
	cerr << "Host Denied!\n";
#endif
	return 0;

}

/* _MatchesIP (protected)
 *
 * return true if a given IP is acceptable to this SURL
 * - See the notes for _MatchesHost() for details.
 *
 * Russell Holt early July 1995
 * - modified July 20 1995 for list classes.
 */
int surl::_MatchesIP(char *ip)
{
#ifdef DEBUG
	cerr << "_MatchesIP() -- n_IPs = " << requireIP.length() << '\n';
#endif

	String rip = ip;

	// if the remote IP contains the required IP starting
	// at position 0, then we're ok.  eg, if the
	// remote IP is 204.120.87.12, and the required IP
	// is 204.120.87, then since this is a substring
	// of 204.120.87.12 beginning at position 0, it is
	// acceptable

	Pix temp = requireIP.first();
	while(temp)
	{
		if (rip.contains(requireIP(temp), 0))
		{
#ifdef DEBUG
			cerr << "Accepted\n";
#endif
			return 1;
		}
		requireIP.next(temp);
	}

#ifdef DEBUG
	cerr << "Denied!\n";
#endif
	return 0;
}


/*
 * _MatchesUser (protected)
 *
 *  Return true (1) if the given user/password are acceptable
 *  to this SURL.
 *
 *  Protected method - interfaced via the MatchesUser inline
 * function to call this method only if there are users to check (speed).
 *
 * Assumes that no users means no authorization for simplicity, and
 * because it is intended to be used through MatchesUser() which does
 * that check.
 *
 * Usernames and passwords must match exactly.
 *
 * Perhaps the userlist will be replaced with a more comprehensive
 * user database type of system.
 *
 * Russell Holt, July 20 1995
 */
int surl::_MatchesUser(String &user, String& passwd)
{
Pix temp = userlist.first();
stringpair u;

	cerr << "_MatchesUser()\n";

	while(temp)
	{
		u = userlist(temp);
		if (u.left() == user && u.right() == passwd)
			return 1;

		userlist.next(temp);
	}

	return 0;
}


void surl::AddRequiredIP(char *ip)
{
	requireIP.append(String(ip));
}
void surl::AddRequiredIP(String &ip)
{
	requireIP.append(ip);
}
void surl::AddRequiredHost(char *ho)
{
	requireHost.append(String(ho));
}
void surl::AddRequiredHost(String &ho)
{
	requireHost.append(ho);
}

/* AddRequiredUser
/* Note that when there are no required users, NOBODY requires
 * a password.  But by adding a required user, then ONLY that
 * user now has access.  Adding more required users just makes
 * a list of those who have access.
 */
void surl::AddRequiredUser(char *username, char *passwd)
{
	userlist.append(stringpair(String(username), String(passwd)));
}
void surl::AddRequiredUser(String& username, String& passwd)
{
	userlist.append(stringpair(username, passwd));
}



/* print
 *
 * print the surl info to stdout
 *
 */
void surl::print(void)
{
	cout << url << " : " << WhatType() << (visible?" ":" in") << "visible ";
	cout << (active?"":"in") << "active ";
	cout << "\"" << title << "\" ";
}

/*-----------------------------------------------------*/
/* File SURL
/*-----------------------------------------------------*/

//--- url, type ------
file_surl::file_surl(String& url, int type) : surl(url, type)
{
	//	title = NULL;
}

//--- url, path -------
file_surl::file_surl(String& url, String& f) : surl(url, type_FILE)
{
	path = f;
	//	title = NULL;
}

//--- url, path, title --------
file_surl::file_surl(String& url, String& f, String& title)
	: surl(url, type_FILE, title)
{
	path = f;
}

//--- url, path, type ---------
file_surl::file_surl(String& url, String& f, int type)
: surl(url, type)
{
	path = f;
}

//--- url, path, type, title --------
file_surl::file_surl(String& n, String& f, int t, String& tt)
	: surl(n, t, tt)
{
	path = f;
	visible=1;
	active=1;
}

//---- url, path, type, title --------
file_surl::file_surl(char *n, char *f, int t, char *tt)
	: surl(n, t, tt)
{
	path = f;
	visible=1;
	active=1;
}



void file_surl::print(void)
{
	surl::print();
	cout << path;
}


/*
// cgi_surl obsolete
void cgi_surl::print(void)
{
	surl::print();
}
*/

// WhatType
// translate the type member field to a char *
const char *surl::WhatType(void)
{
	if (type < MaxTypes)
		return TheTypes[type];

	return NULL;
}


int directory_surl::MatchesUrl(String a_url)
{
	return (url.contains(a_url, 0));
}
