// SLList_util.h

#ifndef _SLLIST_UTIL_H_
#define _SLLIST_UTIL_H_

/*
 *  Some useful StringSLList things
 * Russell Holt May 1995
 *
 */

#include <stream.h>
#include <fstream.h>
#include <String.h>
#include <SLList.h>
#include "stringpair.h"

/* Read each line from an ifstream into a String and
 *  append it to a StringSLList.
 *
 * Russell Holt, May 17 1995
 * Modified for SLList<String> July 28 1995
 * fixed to work Oct 25 1995
 */
inline ifstream& operator >> (ifstream& in, SLList<String>& alist)
{
char x[1024];
int i;
String thestring;
_IO_size_t len, Max = 1024;

	while (!in.eof())
	{
		for(i=0; i<1024; i++) x[i]='\0';	// initialize buffer

		// handle arbitrarily long lines without specifying length
		do {
			in.getline(x, Max);
			len = in.gcount();
			thestring += x;
							// 1024 total length, 1023 would hold '\0'
		} while (len >= Max && x[Max-1] != '\n' );

		if (in.eof())
			break;

		thestring.at("\n", -1) = "";
		alist.append(thestring);
		thestring = "";
	}

	return in;
}

inline ostream& operator<<(ostream& out, SLList<String> list)
{
Pix temp = list.first();

	while(temp)
	{
		out << list(temp) << '\n';
		list.next(temp);
	}

	return out;
}

inline ofstream& operator<<(ofstream& out, SLList<String> list)
{
Pix temp = list.first();

	while(temp)
	{
		out << list(temp) << '\n';
		list.next(temp);
	}

	return out;
}

/*
 * Send a SLList<stringpair> over an ostream.
 * Uses the form "(left, right)"
 */
inline ostream& operator<<(ostream& out, SLList<stringpair> list)
{
Pix temp = list.first();
stringpair a;

	while(temp)
	{
		a = list(temp);
		out << '(' << a.left() << ", " << a.right() << ")\n";
		list.next(temp);
	}
}

#endif
