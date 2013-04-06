/* cgirtest
 *
 * A cgi executable
 *
 * Uses the cgi resource subclass to do cgi interfacing and
 * decoding.
 *
 * Russell Holt July 19, 1995
 */
extern "C"
{
#include <stdio.h>
}
#include <stream.h>
#include "cgi.h"
#include <SLList.h>
#include "stringpair.h"

main(int argc, char *argv[])
{
cgi abc;
SLList<stringpair> cgilist;

	abc.SendType("text/html");

	abc.LoadData();

	abc.ParseCGIData(cgilist);

	
	cout << "<title>An OO cgi test</title>\n";
	cout << "<h1>An OO cgi test</h1><hr>\n";

	// actually could be a call to a list-to-HTML-table resource
	abc.Test();

}
