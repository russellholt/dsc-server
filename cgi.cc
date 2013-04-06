#include "cgi.h"
#include <ctype.h>

cgi::cgi(String *data)
{
	if (data)
		cgi_data = *data;
}

cgi::cgi(char *data)
{
	cgi_data = data;
}

cgi::cgi(void)
{ }

int cgi::LoadData(void)
{
char *method = NULL;

	method = getenv("REQUEST_METHOD");

	if (!method)
		return 0;	// error

	if (strcmp(method, "GET") == 0)
	{
		Get();
		return 1;	// ok
	}
	else
	if (strcmp(method, "POST") == 0)
		printf("The POST method is not currently implemented.");
	else
		printf("This script was referenced with the unknown method %s.\n",
			method);
			
	return 0;	// error
}

// GET specific stuff
void cgi::Get(void)
{
char *query_string = NULL;
	query_string = getenv("QUERY_STRING");
	if (query_string)
		cgi_data = query_string;
	
}

// POST specific stuff
void cgi::Post(void)
{ }

/* ParseCGIData
 *
 *  Add name (left), value (right) String pairs to
 *  an SLList<stringpair>.
 *
 *  The format is name=value pairs separated by &
 *  with spaces turned into '+'.  '%' indicates that the
 *  next 2 chars are a hex value.
 *
 * Russell Holt May 18 1995
 * totally rewritten Oct 10 1995 to make better use of String functions
 */
void cgi::ParseCGIData(SLList<stringpair>& cgilist)
{
	if (cgi_data.length() == 0) return;

	int i = cgi_data.freq("&") + 1, i2, x;

	String list[i+1];
	i2 = split(cgi_data, list, i, String("&"));
	
	String left, right;
	stringpair *apair;
	for (x = 0; x<i2; x++)
	{
		list[x].gsub("+", " ");
		left = unescape_hex(list[x].before("="));
		right = unescape_hex(list[x].after("="));
		apair = new stringpair(left, right);
		cgilist.append(*apair);
	}
}

/* unescape_hex
 * Convert all occurrences of '%' followed by two hex digits
 * with the ascii equivalent, ie "%2f" -> '/'
 * (used to be "TransformCGI")
 * Written Oct 10 1995, RFH
 */
String cgi::unescape_hex(String h)
{
int i, li=0;
String uesc;
char lsd, msd;

	while ((i = h.index(esc_hex, li)) >= 0)
	{
		uesc = h.at(i, 3);	// %xx
		msd = tolower(uesc[1]);	// most significant digit
		lsd = tolower(uesc[2]); // least significant digit
		if (msd > 0 && lsd > 0)	// my hex converter
			h.at(i,3) = (char) (hexdig.index(msd) * 16 + hexdig.index(lsd));
		li = i;
	}
	return h;
}


/* ShowData
 *
 * Test cgi program
 * RHolt July 1995 (but mostly taken from httpResHandler)
 */
void cgi::Test()
{
SLList<stringpair> &cgilist = *thelist;
Pix temp=cgilist.first();
stringpair a;
String cginame, cgival;

	// cgi data printing routine taken from httpResHandler.cc

	cout << "<center><table border=3 cellpadding=3>\n"
		<< "<tr><td colspan=2><b>Submitted CGI data</b></td></tr>"
		<< "<tr><td align=center><b>Name</b></td>"
		<< "<td align=center><b>Value</b></td></tr>";

	while (temp)
	{
		a = cgilist(temp);
		cginame = a.left(); cgival = a.right();	// not really necessary

		cout << "<tr><td>" << cginame << "</td><td>";
		cout << cgival << "<br></td></tr>";

		cgilist.next(temp);
	}
	cout << "</table></center><p>";


}

// GetServerVars
// - add the environment variables set by the server when this cgi
//   program was launched to var_list passed in.
// 
// RFH 9/14/95
void cgi::GetServerVars(SLList<stringpair>& var_list)
{
int i;
char *env_string;
String thevalue;
stringpair sp;

	for(i=0; i<N_ENV_VARS; i++)
	{
		env_string = getenv(server_env[i]);
		if (env_string)
		{
			//	thestring = new String(query_string);
			thevalue = env_string;
			sp.SetLeft(server_env[i]);
			sp.SetRight(thevalue);
			var_list.append(sp);
		}
	}
}
