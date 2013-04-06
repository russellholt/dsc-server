/********************************************************
 * cgimain.c - where it all begins
 * This is 80% NCSA source.  Destiny changes:
 *
 * > Reduced entry allocation from 10000 to 100;
 *
 * > Added a call to the external function DoCGIThing()
 *     to do whatever is compiled with this code.
 *
 * > other junk starting in June 1995
 *
 * - Russell Holt, Destiny Software Corporation
 ********************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "entry.h"

void getword();
char x2c();
void unescape_url();
void plustospace();

/* external CGI function- whatever is compiled with this file */
extern void DoCGIThing();

main(argc, argv)
int argc;
char *argv[];
{
    entry entries[100];		/* dist code had 10000 */
    register int x,m=-1,y;
    char *cl;

    printf("Content-type: text/html%c%c",10,10);

    if(strcmp(getenv("REQUEST_METHOD"),"GET")) {
        printf("This script should be referenced with a METHOD of GET.\n");
        printf("If you don't understand this, see this ");
        printf("<A HREF=\"http://www.ncsa.uiuc.edu/SDG/Software/Mosaic/Docs/fill-out-forms/overview.html\">forms overview</A>.%c",10);
        exit(1);
    }

    cl = getenv("QUERY_STRING");

    if (cl != NULL)
		for(x=0;cl[0] != '\0';x++)
		{
			m=x;
			getword(entries[x].val,cl,'&');
			plustospace(entries[x].val);
			unescape_url(entries[x].val);
			getword(entries[x].name,entries[x].val,'=');
		}


	/* Do It */

	DoCGIThing(entries, m);
}


