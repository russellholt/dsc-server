/************************************** *** ***
 *
 * SURL
 *
 * Russell Holt, June/July 1995
 ************************************** *** ***/

#ifndef _SURL_H_
#define _SURL_H_ 1

#include <SLList.h>
#include <String.h>
#include <stream.h>
#include <ostream.h>
#include "stringpair.h"

#include "../crb/resource.h"
#include "../crb/restable.h"

extern "C"
{
#include <string.h>
}
#define untyped 0
#define type_BuiltIn 1
#define type_FILE 2
#define type_CGIInternal 3
#define type_CGIExternal 4
#define type_ForwardRequest 5
#define type_Special 6
#define type_Directory 7
#define type_DirectoryLink 8
#define type_META 11
#define type_Location 12
#define type_rsl 13

#define MaxTypes 14

//#define surl_visible 1
//#define surl_active 2

class surl {
	int _MatchesIP(char *ip);
	int _MatchesHost(char *ho);
	int _MatchesUser(String& user, String& passwd);

protected:
	int type;
	int visible;
	int active;
	int show_modified_date;
	String url;
	String title;
	String referer;

	SLList<String> requireIP, requireHost;
	SLList<stringpair> userlist;
	
	resource *dynamic;
	Init(void);
	
public:
	surl(void);
	surl(String& n, int t);
	surl(char *n, int t);
	surl(String& n, int t, String& tt);
	surl(char *n, int t, char *tt);

	~surl(void);

	inline int totalRequiredIPs(void)
	{ return requireIP.length(); }

	inline int totalRequiredHosts(void)
	{ return requireHost.length(); }

	inline int totalRequiredUsers(void)
	{ return userlist.length(); }

	inline int MatchesIP(char *ip)
	{	return (requireIP.length()>0? _MatchesIP(ip) : 1);	}

	inline int MatchesHost(char *ho)
	{	return (requireHost.length()>0? _MatchesHost(ho) : 1);	}

	inline int MatchesUser(String& user, String& passwd)
	{	return (userlist.length()>0? _MatchesUser(user, passwd) : 1);	}

	inline resource *Resource(void) { return dynamic; }
	inline void SetResource(resource *r) { dynamic = r; }
	
	void AddRequiredIP(char *ip);
	void AddRequiredIP(String &ip);
	void AddRequiredHost(char *ho);
	void AddRequiredHost(String &ho);
	void AddRequiredUser(String& username, String& passwd);
	void AddRequiredUser(char *username, char *passwd);

	inline int IsVisible(void) { return visible; }
	inline void SetVisibility(int x) { visible = x; }
	inline int IsActive(void) { return active; }
	inline void SetActive(int x) { active = x; }

	inline char Type(void) { return type; }
	inline char SetType(int t) { type = t; }

	inline String& Url(void) { return url; }
	inline void SetUrl(String &nm) { url = nm; }
	virtual int MatchesUrl(String a_url);
	
	inline String& Title(void) { return title; }
	inline void SetTitle(String &t) { title = t; }
	
	inline String& Referer(void) { return referer; }
	inline void SetReferer(String &rf) { referer = rf; }

	virtual void print(void);
	virtual int Action(void)
		{ cout << "surl::Action() for " << url << '\n'; return 0; }

	const char *WhatType(void);
	friend inline ostream& operator << (ostream& o, surl s);
};

inline ostream& operator<<(ostream& o, surl s)
{
	o << s.url << '(' << s.type << ')';
	return o;
}

//------------------------------------------

class file_surl : public surl {
protected:
	String path;

public:
	file_surl(void) { }
	file_surl(String& url, int type=type_FILE);
	file_surl(String& url, String& f);
	file_surl(String& url, String& f, String& title);
	file_surl(String& url, String& f, int type);
	file_surl(String& n, String& f, int t, String& tt);
	file_surl(char *n, char *f, int t, char *tt);

	void print(void);
//	int Action(void) { return 0; }

	inline void SetFile(String &f) { path = f; }
	inline const char *File(void) { return path.chars(); }
	inline String sFile(void) { return path; }
	
	friend inline ostream& operator << (ostream& o, file_surl& s);
};

inline ostream& operator<<(ostream& o, file_surl& s)
{
    o << s.url << " FILE (" << s.type << ')';
    return o;
}

//------------------------------------------

class directory_surl : public file_surl {
public:
	directory_surl(void) { }
	
	int MatchesUrl(String a_url);
	friend inline ostream& operator<<(ostream& o, directory_surl& s);
};

inline ostream& operator<<(ostream& o, directory_surl& s)
{
    o << "directory " << s.url << " allow links: "
    	<< (s.type == type_DirectoryLink ? "yes" : "no");
    return o;
}

//------------------------------------------

//class cgi_surl : public surl {
//protected:
//	int a;
//public:
//	cgi_surl(String& str) : surl(str, type_CGIInternal)
//		{ }
//	cgi_surl(String& str, String& g) : surl(str, type_CGIInternal, g)
//		{ }
//
//	void print(void);
//	int Action(void)
//		{ cout << "cgi_surl::Action for " << url << '\n'; return 0; }
//
//	friend inline ostream& operator << (ostream& o, cgi_surl s);
//
//};
//
//inline ostream& operator<<(ostream& o, cgi_surl s)
//{
//    o << s.url << " CGI (" << s.type << ')';
//    return o;
//}

#endif
