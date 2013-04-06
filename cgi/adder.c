/*
 *  adder.c
 *
 *  A cgi program to append text to a file, like a bbs.
 *  The file is specified by the form, so multiple "boards" may
 *  be maintained.
 *
 * Russell Holt June 1995
 * Modified for multiple boards July 6 1995
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "entry.h"

#ifndef debug
#define debug 0
#endif


/* DoCGIThing: the 'main' routine; called from cgimain.c */
void DoCGIThing(entries, m)
entry entries[];
int m;	/* max */
{
char *cl, *name, *email, *thefile, *message;
char newfile[120];
char syscl[120];
int x;
FILE *fp;

/*
	printf("<title>Post-it</title>");
    printf("<center><H1>Post-it</H1><HR></center>");
*/


	for(x=0; x<=m; x++)
	{
		if (strcmp(entries[x].name, "thefile") == 0)
			thefile = entries[x].val;
		else
			if (strcmp(entries[x].name, "name") == 0)
				name = entries[x].val;
		else
			if (strcmp(entries[x].name, "email") == 0 && 
				strlen(entries[x].val) > 0)
				email = entries[x].val;
		else
			if (strcmp(entries[x].name, "message") == 0)
				message = entries[x].val;
	}

	printf("Location: /showboard\n\n");
	printf("<title>Go to...</title>");
	printf("You man now proceed to <a href=\"/showboard\">The Board</a>");

	sprintf(newfile, "/www/htdocs%s", thefile);

	fp = fopen(newfile, "a");

	if (fp)
	{
		fprintf(fp, "<hr>\n");
		fprintf(fp, "<b><font size=4>%s</font></b>\n", name);
		fprintf(fp, " <b><font size=3>(%s)</font></b>\n", email);
		fprintf(fp, "<p>%s\n", message);

		fclose(fp);

/*
		printf("Thank you.  You've posted:<p>\n");

		printf("<center><table border=2 cellpadding=3><tr>");
		for (x=0;x<=m;x++)
		{
			printf("<td align=right><b>%s</b></td> ", entries[x].name);
			printf("<td>%s<br></td></tr>\n",entries[x].val);
		}
		printf("</table></center>\n");

		printf("<p><a href=\"/showboard\">The Board</a><p>");
*/

	}
	else
	{
		puts("Content-type: text/html\n");
		puts("\n<title>Error</title><hr>");
		puts("The command was unable to be completed, because ");
		puts("the Galileo probe is out of range.");
	}

	printf("<HR>");
}

