#include <stream.h>
#include "SLList_util.h"
#include <String.h>
#include <SLList.h>

extern int checkline(char *input, double testpoint[2], String &Default);
extern void sendmesg(char *url);
extern void servererr(char *msg);

main()
{
char *qs = (char *) getenv("QUERY_STRING"),
	 *mapfile = (char *) getenv("PATH_TRANSLATED"),
	 *comma = NULL;
double x, y, testpoint[2];
char def[500];
String Default;

	comma = (char *) index(qs, ',');

	if (comma)
	{
		*comma = '\0';
		comma++;

		/*
		cout.form("The coordinates are, again, '%s' and '%s'<p>\n",
			qs, comma);
		*/

		x = (double) atoi(qs);
		y = (double) atoi(comma);
		testpoint[0] = x;
		testpoint[1] = y;

		/*
		cout.form("The coordinates are, again, (%lf, %lf)<p>\n", x, y);
		*/
	}

	SLList<String> lines;
	ifstream in(mapfile);
	if (!in)
		exit(1);

	in >> lines;

Pix temp = lines.first();
String input;
int done = 0;

	while(temp)
	{
		input = lines(temp);

		done = checkline(input.chars(), testpoint, Default);
		if (done)
			break;

		lines.next(temp);
	}

	if (!done)
	{
		if (Default.length())
		{
			sendmesg(Default.chars());
			/*
			cout << "Content-type: text/html\n\n";
			cout << "default URL hit --> " << Default << '\n';
			*/
		}
		else
			servererr("No default specified.");
	}
}
