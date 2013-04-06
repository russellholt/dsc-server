/*
 * $Id: htmltable.h,v 1.2 1995/09/06 20:13:16 holtrf Exp $
 */
#ifndef _HTML_TABLE_
#define _HTML_TABLE_

#include <stream.h>
#include <stdarg.h>

#include <String.h>
#include <SLList.h>

#include "ohtml.h"
#include "SLList_util.h"

class htmltable {
	SLList<String> Rows;
	String currentRow;
	int center;	// center the table or not
	int width;	// percent of the window width the table should occupy
	int cellpadding;	// space between cells
	int cellspacing;
	int border;	// size of border

public:

	htmltable(void);
	htmltable(int acenter, int awidth, int acellpadding, int aborder);
	void AddRow(int n, ...);
	void AddCharRow(int n, ...);

	inline void NewRow(void) { currentRow = ""; }

	void AddToCurrentRow(int n, ...);
	void AddToCurrentRow(String& td, int align=0, int span=1);
	inline void AddToCurrentRow(String* td, int align=0, int span=1)
			{ if (td) AddToCurrentRow(*td, align, span); }
	inline void AddToCurrentRow(char *td, int align=0, int span=1)
		{ String s = td; AddToCurrentRow(s, align, span); }

	inline void _AddToCurrentRow(String& td)
		{ currentRow += td; }

	inline void AddCurrentRow(void) { Rows.append(currentRow); NewRow(); }

/*
	String *tag(String *s, char *tg);
	String *tagData(String *s, char *tg, int n, ...);
	String *openTagData(char *tg, int n, ...);

	inline String *openTag(char *tg)
			{ String *s = new String(tg);
				s->prepend('<'); *s += '>';
				return s;
			}
	inline String *CloseTag(char *tg)
			{	String *s = new String(tg);
				s->prepend("</"); *s += ">";
				return s;
			}
*/

	void Print(void);
	String *tabledata(String &td, int align, int span);

/*
	inline String *Bold(String *s) { return tag(s, "b"); }
	inline String *Italic(String *s) { return tag(s, "i"); }
*/

	inline int Center() { return center; }
	inline int Width() { return width; }
	inline int Cellpadding() { return cellpadding; }
	inline int Border() { return border; }

	inline void SetCenter(int x) { center=x; }
	inline void SetWidth(int x) { width=x; }
	inline void SetCellPadding(int x) { cellpadding=x; }
	inline void SetCellSpacing(int x) { cellspacing=x; }
	inline void SetBorder(int x) { border=x; }

	SLList<String> *GetTable(void);
};

#endif
