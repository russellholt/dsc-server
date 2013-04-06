/*
 * $Id: data2html.cc,v 1.1 1995/09/06 20:13:16 holtrf Exp $
 */
#include <stream.h>
#include "data2html.h"
#include "htmltable.h"


/* KeyValue2Table
 *
 *	Take a list of "keyword: value" lines, and
 *	turn it into an HTML table.
 *
 *	- Values can be multi-line as long as the succeeding
 *	  lines begin with a space or a tab.
 *
 *	- Lines beginning with '#' are ignored.
 *
 * Here is an example:

keyword1: the value
keyword2: a multi-
	line
	value
keyword3: another value

 *
 * Russell Holt Sept 6, 1995
 */
SLList<String> *KeyValue2Table(SLList<String> &l)
{
Pix temp = l.first();
String s;
String left, right;
htmltable *tb = new htmltable;
SLList<String> *wholelist = new SLList<String>;

	tb->SetBorder(1);

	while(temp)
	{
		s = l(temp);
		if (!s.length() || s[0] == '#')	// skip blank lines & comments
			goto nextone;

		if (s[0] == ' ' || s[0] == '\t')	// begins with whitespace
			right += s;
		else
		{
			if (left.length())	// 'left' should exist but blank 'right' is ok.
			{
				right += "<br>";	// help table-less browsers
				tb->AddRow(2, html_Bold2(&left), &right);
				left = right = "";
			}

			if (!s.contains(":"))	// no colon = no keyword/value pair
				wholelist->append(s);	// so keep it for later
			else
			{
				left = s.before(":");
				right = s.after(":");
				s = "";
			}
		}

nextone:
		l.next(temp);
	}

	// Add the last one !!!!
	if (left.length())	// 'left' should exist but blank 'right' is ok.
	{
		right += "<br>";	// help table-less browsers
		tb->AddRow(2, html_Bold2(&left), &right);
	}

	SLList<String> *tbl = tb->GetTable();
	wholelist->append(String("<br>"));
	wholelist->join(*tbl);

	//	return tb->GetTable();
	return wholelist;
}
