/* Data2HTML
 *
 * Routines for converting generic 'data' to HTML lists or tables
 *
 * $Id: data2html.h,v 1.1 1995/09/06 20:13:16 holtrf Exp $
 *
 * Russell Holt Sept 6, 1995
 */
#ifndef _DATA_2_HTML_
#define _DATA_2_HTML_

#include <String.h>
#include <SLList.h>


SLList<String> *KeyValue2Table(SLList<String> &l);

#endif
