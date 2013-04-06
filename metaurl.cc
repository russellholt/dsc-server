/* the metaURL
 *
 * a list of SURLs
 *
 * Should be re-written to use a binary search tree or some other such
 * more efficient structure
 *
 * Russell Holt
 */
#include "metaurl.h"

#include "surl.h"

metaurl::metaurl(String& MU)
{
	MetaURL = MU;
	count = visible_count = 0;
	head = tail = NULL;
}

metaurl::~metaurl(void)
{
	clear();
}

/*
 *  find
 */
surl *metaurl::operator()(String& name)
{
surl *a;
			// special reference to the whole surl collection
//	if (name == MetaURL)	// hack
//	{
//		String s = "The MetaURL";
//		a = new surl(s, type_META);
//		return a;
//	}

	metaurl_node *temp = head;
	String tname;
	int pos;

	while(temp)
	{
		a = temp->elem;
//		tname = a->Name();

// pre directory_surl way of checking name
//
//		if (a->Type() == type_Directory || a->Type() == type_DirectoryLink)
//		{
//			// if tname has the substring name at position 0
//			if (name.contains(tname, 0))
//				return a;
//		}
//		else
//			if (tname == name)
//				return a;
		if (a->MatchesUrl(name))	// ask the surl if it matches
			return a;

		temp = temp->next;
	}

	return NULL;
}

int metaurl::add(surl *s)
{
	if (!s)
		return 0;

	metaurl_node *temp, *newentry = new metaurl_node;

	newentry->elem = s;
	newentry->next = NULL;
	
	if (head == NULL)
		head = tail = newentry;
	else
	{
		if (tail)
		{
			tail->next = newentry;
			tail = newentry;
		}
		else	// tail gone?? fix it: find end, add newentry, assign tail.
		{
			temp = head;
			while (temp->next)
				temp = temp->next;
			tail = temp->next = newentry;
		}
	}
	count++;
	if (s->Visible())
		visible_count++;
	return 1;
}


int metaurl::insert(surl *s)
{
	if (!s)
		return 0;

	metaurl_node *newentry = new metaurl_node(s, NULL);
	
	if (head == NULL)
		head = tail = newentry;
	else
	{
		newentry->next = head;
		head = newentry;
	}

	count++;
	return 1;
}


int metaurl::del(String& name)
{
metaurl_node *temp = head;
metaurl_node *prev = head;

	while (temp)
	{
		if (name == temp->elem->Name())
		{
#ifdef DEBUG
			cout << "metaurl::del: Found " << name << '\n';
#endif
			if (prev == temp)	// delete the first element
			{
				head = temp->next;
				if (tail == temp)	// only 1 element list
					tail = head = NULL; // make sure to zero them
			}
			else
			{
				if (tail == temp)
					tail = prev;
				prev->next = temp->next;	// skipping over temp
			}

			delete temp;	// kill it!
			count--;
			break;
		}
		prev = temp;
		temp = temp->next;
	}
}

void metaurl::print(void)
{
metaurl_node *temp = head;
int x=0;

	cout << "MetaURL\n";
	cout << count << " elements, " << visible_count
		<< " visible elements.\n";

	while (temp)
	{
		cout << form("%3d: ", x++);
		temp->elem->print();
		cout << '\n';

		temp = temp->next;
	}
}

void metaurl::clear(void)
{
metaurl_node *temp = head;

	while (temp)
	{
		delete temp->elem;
		temp = temp->next;
	}
}


/* - - - - - - - - - - - - - - - - - - - -
 *  Send the metaURL table over a Socket
 *
 *  Only SURLs are sent which
 *  - match the remote IP if required
 *	- are visible
 *
 * Russell Holt, May or June 1995
 * IP matching added July 11 1995
 * - - - - - - - - - - - - - - - - - - - - */
void metaurl::Send(Socket& sock)
{
metaurl_node *temp = head;
//	String s, *g;
String s, g;
int x = 0;

	print();

/*
    sock << "<b>" << MetaURL << "</b><p>";
	sock << String(dec(count)) << " entries, ";
	sock << String(dec(visible_count)) << " visible.<p>";
*/

	sock << "<center>";
    sock << "<table cellpadding=3 cellspacing=1 border=1>\n";
    while(temp)
    {
				//-- must match IP to appear in metaURL table
		if (temp->elem->MatchesIP(sock.RemoteIP()))
		if (temp->elem->MatchesHost(sock.RemoteHost()))

				//-- must be visible to appear in table
			if (temp->elem->Visible())
				{
					s = temp->elem->Name();
					g = temp->elem->Title();

				/*
					sock << s;
					if (g)
						sock << "  " << *g;
					sock << "<br>";
				*/

					sock << "<tr><td> " << String(dec(x))
						 << " </td><td align=middle >";
					sock << "<a href=\"" << s << "\">";

					//	sock << (g ? (*g) : s) << "</a>";
					sock << (g.length() ? g : s) << "</a>";

					//	sock << s << "</a>";
					sock << "<br></td></tr>";

					x++;	// only inc if visible!
				}
		temp = temp->next;
    }
    sock << "</table><p>";
	sock << "</center>";
}

/*
void metaurl::iterate((void *) fn()(surl *s) )
{
metaurl_node *temp = head;

	while(temp)
	{
		fn(temp->elem);
		temp = temp->next;
	}
}
*/
