/* MIME stuff
 *
 * Basically a front end to the singly linked list of mime element
 * nodes which contains 2 strings: filename extension & MIME type.
 *
 * This should be modified to use a binary search tree or some other
 * more efficient structure. (won't be hard)
 *
 * Russell Holt, May 8 1995
 *
 * Changes:
 *
 * July 29, 1995:
 * - Replaced stringpairSLList class with SLList<stringpair>
 *   template class.
 *
 * May 18
 * - eliminated the mime_elem and mime_elemSLList types...
 *   replaced with more generic and better stringpair type.
 * - added AddType so that you can easily add MIME types to
 * the list that are not in the config file.
 *
 */

#include <SLList.h>
#include <stream.h>
#include <fstream.h>
#include <String.h>

#include "Mime.h"
#include "stringpair.h"

extern "C" {
#include <ctype.h>
}


Mime::Mime(char *filename)
{
	configFileName = filename;
}

/*
 * Reads a mime configuration file and parses each line into
 * a MIME type and a filename extension.  Blank lines and lines 
 * beginning with `#' are ignored.
 *
 * If no config file is found, a "minimal" mime list is created
 * (see CreateMinimalList).
 *
 * Russell Holt, May 8,9 1995
 */
void Mime::ReadConfigFile(void)
{
ifstream in(configFileName.chars());

	if (!in)
	{
		CreateMinimalList();
		return;
	}

char x[512];
stringpair e;
int len;

	while (!in.eof())
	{
		in.getline(x, 512);

		if (in.eof())
			break;

		//	s = x;
		len = strlen(x);

		// blank line (newline is first char) or comment
		if (!len || (x[0] == '#' || x[0] == '\n'))
			continue;

		char *c=x, *ext;

		// search from the beginning until a non-space char is found
		// (including tabs, etc)
		for(c=x; *c && !isspace(*c); c++) ;

		if (!(*c))
			continue;	// no extension found: skip it

		*c++ = 0;	// null terminate first string & skip the zero
		e.SetRight(x);	// content type

		do {
			for(c; *c && isspace(*c); c++) ;	// skip whitespace
			ext = c;	// the beginning of the next extension

			if (!(*c))	// no extension found, so don't add this
				break;	//  MIME type. We couldn't use it anyway.

			// cout << "[ looking at ";
			for(; *c && !isspace(*c); c++) ;
				//	cout << *c;
			//	cout << " ] ";
			if (*c)
				*c++ = 0;	// null terminate the string then increment
							// to skip over the zero in the next pass!

			//	cout << " ext: \"" << ext << "\"";

			e.SetLeft(String(ext));	// extension
			list.append(e);
		} while(*c);

	}

#ifdef DEBUG
	cout << "Finding content type for `gif':";
	String xyz, gif = "gif";
	int l = GetType(gif, xyz);
	cout << xyz << "\n";
#endif
}

/* AddType
 *  Add a filename extension and mime type to the list, if
 *  it is not already there.
 *  returns 1 on success
 *          0 if extension is already in the list
 *
 * Russell Holt, May 18 1995
 */
int Mime::AddType(String& extension, String& content_type)
{
String x;

	if (!GetType(extension, x))	// extension not in our list
	{
	stringpair e;
		e(extension, content_type);
		list.append(e);
		return 1;	// success
	}

	return 0;	// don't add - extension already in the list.
}

/*
 * find a MIME type for a given extension
 * returns 1 for success, 0 for fail.
 * Russell Holt, May 8,9 1995
 */
int Mime::GetType(String& extension, String& content_type)
{
String it;
stringpair a;
Pix temp = list.first();

	while(temp)
	{
		a = list(temp);
		if (a.left() == extension)	// has to match exactly
		{
			content_type = a.right();
			return 1;
		}
		list.next(temp);
	}

	// the filename extension is not found
	content_type = "";
	return 0;
}

int Mime::GetExtension(String& content_type, String& extension)
{
	return 0;
}

/*
 * No mime config file found, so create a minimal list
 * Should log this error.
 */
void Mime::CreateMinimalList(void)
{
stringpair a;
	
#ifdef DEBUG
	cerr << "MIME: Unable to open MIME types configuration file."
		<< "  Using minimal set of `text/html', `image/gif', `text/plain'.\n";
#endif

	QuickAddType("html", "text/html");
	QuickAddType("gif", "image/gif");
	QuickAddType("jpeg", "image/jpeg");
	QuickAddType("txt", "text/plain");
}
