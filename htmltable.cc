/*
 * $Id: htmltable.cc,v 1.2 1995/09/06 20:13:16 holtrf Exp $
 */
#include "htmltable.h"

// we may not care about any of those formatting twiddlers
htmltable::htmltable(void)
{
	center = -1;
	width = -1;
	cellpadding = -1;
	border = -1;
	cellspacing = -1;
}

htmltable::htmltable(int acenter, int awidth, int acellpadding, int aborder)
{
	center = acenter;
	width = awidth;
	cellpadding = acellpadding;
	border = aborder;
}

// add a row of String * items
// n is the number of arguments that follow
void htmltable::AddRow(int n, ...)
{
va_list ap;
String row;

	va_start(ap, n);

	for(;n--;)
	{
		String data = *va_arg(ap, String *);
		row += *(html_tag(&data, "td"));
	}

	va_end(ap);

	Rows.append(row);
}

// add a row of char * items
void htmltable::AddCharRow(int n, ...)
{
va_list ap;
String a, row;

	va_start(ap, n);

	for(;n--;)
	{
		a = va_arg(ap, char *);
		row += *(html_tag(&a, "td"));
	}

	va_end(ap);

	Rows.append(row);
}

// add a complex item to current row
void htmltable::AddToCurrentRow(String& td, int align, int span)
{
	String *s = tabledata(td, align, span);
	currentRow += *s;
}

// Add several simple items to the current row
void htmltable::AddToCurrentRow(int n, ...)
{
va_list ap;
String row;

	va_start(ap, n);

	for(;n--;)
	{
		String data = *va_arg(ap, String *);
		row += *(html_tag(&data, "td"));
	}

	va_end(ap);

	currentRow += row;
}

String *htmltable::tabledata(String& td, int align=0, int span=1)
{
String *s = new String(td);

String al, sp;

	if (align > 0 && align < 3)
		al = String("align=") + alignment[align];
	if (span > 1)
		sp = String("colspan=") + dec(span);
	
	// Make table data
	s = html_tagData(s, "td", 2, &al, &sp);

	return s;
}


SLList<String> *htmltable::GetTable(void)
{
String cent, wid, pad, spac, bord, *opent;

	if (width >= 0)
		wid = String(" width=") + dec(width);
	if (cellpadding >= 0)
		pad = String(" cellpadding=") + dec(cellpadding);
	if (cellspacing >= 0)
		spac = String(" cellspacing=") + dec(cellspacing);
	if (border >= 0)
		bord = String(" border=") + dec(border);

	opent = html_openTagData("table", 4, &wid, &pad, &spac, &bord);

	SLList<String> *Table = new SLList<String>;
	String arow;

	Table->append(*opent);		// open the table

	Pix temp = Rows.first();
	while(temp)
	{
		arow = "<tr>";
		arow += Rows(temp);
		arow += "</tr>";
		Table->append(arow);
		Rows.next(temp);
	}
	Table->append(String("</table>"));	// close the table

	return Table;
}


void htmltable::Print(void)
{
Pix temp = Rows.first();

	cout << "<table";
	if (width >=0)
		cout << " width=" << width;
	if (cellpadding >=0)
		cout << " cellpadding=" << cellpadding;
	if (cellspacing >=0)
		cout << " cellspacing=" << cellspacing;
	if (border >=0)
		cout << " border=" << border;

	cout << ">\n";

	while(temp)
	{
		cout << "<tr>" << Rows(temp) << "</tr>\n";
		Rows.next(temp);
	}

	cout << "</table>\n";
}
