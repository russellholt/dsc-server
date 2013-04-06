#ifndef _META_URL_H_
#define _META_URL_H_ 1

#include "surl.h"
#include <stream.h>
#include "Socket.h"

class surl;

class metaurl_node {
protected:
	metaurl_node *next;
	surl *elem;
	Init(void) { elem = NULL; next = NULL; }
public:
	metaurl_node(surl *s, metaurl_node *n)
		{ Init();	next = n; elem = s; }

	metaurl_node(void)
		{ Init(); }

	~metaurl_node(void)
		{ delete elem; }

	inline surl *operator()(void)
		{ return elem; }

	inline metaurl_node *Next(void)
		{ return next; }
	
	friend class metaurl;
};

class metaurl {
protected:
	String MetaURL;
	int count;
	int visible_count;
	metaurl_node *head;
	metaurl_node *tail;

public:
	metaurl(String& MU);
	~metaurl(void);

	surl *operator()(String& name);
	int add(surl *s);
	int insert(surl *s);
	int del(String& name);

	void print(void);
	void clear(void);

	inline String& Title(void)
		{ return MetaURL; }

	inline surl *Head(void)
		{ if (head) return head->elem; return NULL; }

	inline surl *Tail(void)
		{ if (tail) return tail->elem; return NULL; }

	//	friend inline ostream& operator << (ostream& o, metaurl m);

	void Send(Socket& sock);
	friend Socket& operator<<(Socket& sock, metaurl mu);
};

inline Socket& operator<<(Socket& sock, metaurl mu)
{
metaurl_node *temp = mu.head;
//	String s, *g;
String s, g;

	sock << "<b>" << mu.Title() << "</b><p>";

	sock << "<table cellpadding=0 border=0>\n";
	while(temp)
	{
		s = temp->elem->Name();
		g = temp->elem->Title();
		sock << "<tr><td>";
		sock << "<a href=\"" << s << "\">";
		//	sock << (g ? (*g) : s) << "</a>";
		sock << (g.length() ? g : s) << "</a>";
		sock << "<br></td></tr>";
		temp = temp->next;
	}
	sock << "</table><p>";
	return sock;
}

/*
inline ostream& operator<<(ostream& o, metaurl m)
{
int x;
 
	for(x=0; x < m.size; x++)
		if (m.table[x])
			o << *m.table[x] << '\n';

	return o;
}
*/

#endif
