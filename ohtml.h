/*
 *  HTML tagging functions
 *  For creating tags like
 *
 *   <a href="http://www.destinyusa.com/">this</a>
 *	 <img border=0 align=middle src="a.gif" ISMAP>
 *
 * $Id: ohtml.h,v 1.2 1995/09/06 20:13:16 holtrf Exp $
 *
 * RFH 8/29-31/95
 * Destiny Software Corporation
 */
#ifndef _O_HTML_H_
#define _O_HTML_H_

#include <String.h>
#include <SLList.h>

#include <stream.h>
#include <stdarg.h>

#define ALIGN_LEFT 0
#define ALIGN_MIDDLE 1
#define ALIGN_RIGHT 2

#define STYLE_BOLD 0
#define STYLE_ITALIC 1
#define STYLE_EM 2
#define STYLE_TT 3
#define STYLE_CODE 4
#define STYLE_PRE 5
#define STYLE_MAX 6

static char *alignment[3] = {"left", "middle", "right"};
static char *stylechars[STYLE_MAX] = {"b", "i", "em", "tt", "code", "pre"};



String *html_tag(String *s, char *tg);
String *html_tag2(String *s, char *tg);
String *html_tagData(String *s, char *tg, int n, ...);

inline String *html_tag1Data(String *s, char *tg, char *data, String *value)
	{
		String v = String(data) + *value;
		return html_tagData(s, tg, 1, &v);
	}

inline String *html_tag1Data(String *s, char *tg, char *data, char *value)
	{
		String v = String(data) + String(value);
		return html_tagData(s, tg, 1, &v);
	}

String *html_openTagData(char *tg, int n, ...);

inline String *html_openTag1Data(char *tg, char *data, String *value)
	{
		String v = String(data) + *value;
		return html_openTagData(tg, 1, &v);
	}

inline String *html_openTag(char *tg)
	{ String *s = new String(tg);
		s->prepend('<'); *s += '>';
		return s;	}

inline String *html_CloseTag(char *tg)
	{	String *s = new String(tg);
		s->prepend("</"); *s += ">";
		return s;	}

inline String *html_Bold(String *s) { return html_tag(s, "b"); }
inline String *html_Bold2(String *s) { return html_tag2(s, "b"); }

inline String *html_Italic(String *s) { return html_tag(s, "i"); }
inline String *html_Italic2(String *s) { return html_tag2(s, "i"); }


#endif
