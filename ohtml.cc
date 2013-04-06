/*
 * $Id: ohtml.cc,v 1.2 1995/09/06 20:13:16 holtrf Exp $
 */
#include "ohtml.h"

String *html_tag(String *s, char *tg)
{
String *ns = new String(*s);

	ns->prepend('>');		//	>abcde
	*ns += "</";				//	>abcde</
	ns->prepend(tg);		//	i>abcde<
	*ns += tg;				//	i>abcde</i
	ns->prepend('<');		//	<i>abcde</i
	*ns += '>';				//	<i>abcde</i>
	return ns;
}

// in-place version
String *html_tag2(String *s, char *tg)
{
	s->prepend('>');		//	>abcde
	*s += "</";				//	>abcde</
	s->prepend(tg);		//	i>abcde<
	*s += tg;				//	i>abcde</i
	s->prepend('<');		//	<i>abcde</i
	*s += '>';				//	<i>abcde</i>
	return s;
}


// html_tagData
// Given a string, surround it with HTML tags that have data values.
// Example: <font size=5>abcde</font> would take
// 		"abcde", "font", 1, "size=5"    as arguments.
//
// Expects String * !!!
//
// RFH 8/25/95
//
String *html_tagData(String *s, char *tg, int n, ...)
{
va_list ap;
String *data;
String t = tg;
String *ns = new String(*s);

	ns->prepend('>');

	va_start(ap, n);

	for (;n--;)
	{
		data = va_arg(ap, String *);
		if (data && data->length())
		{
			ns->prepend(*data);	// data should be of the form "tag=value"
			ns->prepend(' ');
		}
	}
	va_end(ap);

	ns->prepend(tg);		//	i ...>abcde</i>
	ns->prepend('<');		//	<i ...>abcde</i>

	*ns += *(html_CloseTag(tg));

	return ns;
}



// openTagData
// Create an HTML "open" tag with data values
//
// Example, given "font", 1, "size=5",  produce:
//		<font size=5>
//
// Example, given "td", 2, "border=1", "cellpadding=3", produce:
//		<td border=1 cellpadding=3>
//
String *html_openTagData(char *tg, int n, ...)
{
va_list ap;
String *data;
	String *ns = new String('>');	//	>

	va_start(ap, n);
	for (;n--;)
	{
		data = va_arg(ap, String *);
		if (data && data->length())
		{
			ns->prepend(*data);	// data should be of the form "tag=value"
			ns->prepend(' ');	// space to separate it
		}
	}
	va_end(ap);

	ns->prepend(tg);		//	i ...>
	ns->prepend('<');		//	<i ...>

	return ns;
}
