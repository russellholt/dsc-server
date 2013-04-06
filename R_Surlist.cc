#include "R_Surlist.h"
#include <String.h>
#include <SLList.h>
#include "surl.h"

resource *R_Surlist::Create(String &nm, resource *table)
{
	R_Surlist *nr = new R_Surlist(nm);
	return nr;
}

resource *R_Surlist::execute(String& method, SLList<resource *> &args)
{
	method.downcase();
	
	if (method.contains("add", 0))
	{
		String addwhat = method.after(2);
		Add(args, addwhat);
	}
	else
		cout << "Surlist: unknown method \"" << method << "\"\n";
	
	return NULL;
}

void R_Surlist::Add(SLList<resource *> &args, String &addwhat)
{
surl *ns = NULL;
String url, path, title, cgi, index, location;
int visible=1, active=1, allowlinks=0;

#ifdef DEBUG
	cout << "Surlist: Add " << addwhat;
#endif

	if (addwhat == "rsl")
		ns = new surl;
	else
	if (addwhat == "directory")
		ns = new directory_surl;
	else
		ns = new file_surl;

Pix temp = args.first();
resource *r = NULL;
	for(;temp;args.next(temp))
	{
		r = args(temp);
		if (!r) break;

		String resname = downcase(r->Name());
		if (r->ClassName() == "String")
		{
#ifdef DEBUG
			cout << "  " << r->Value();
#endif
			if (resname == "url") url = r->Value();
			else
			if (resname == "path") path = r->Value();
			else
			if (resname == "cgi") cgi = r->Value();
			else
			if (resname == "location") location = r->Value();
			else
			if (resname == "index") index = r->Value();
			else
			if (resname == "requiredhost")
				ns->AddRequiredHost(r->Value());
			else
			if (resname == "requiredip")
				ns->AddRequiredIP(r->Value());
			else
			if (resname == "requireduser")
			{
				String up = r->Value();
				if (up.contains("/"))
				{
					String us = up.before("/"), pw=up.after("/");
					ns->AddRequiredUser(us, pw);
				}
			}
			if (resname == "title") title = r->Value();
			
		}
		else if (r->ClassName() == "Boolean")
		{
			if (resname == "visible") visible = r->LogicalValue();
			else
			if (resname == "active") active = r->LogicalValue();
			else
			if (resname == "allowlinks") allowlinks = r->LogicalValue();
		}
		else
			if (r->ClassName() == "List")
				ns->SetResource(r);
	}

	if (url.length() == 0)
	{
		delete ns;
		return;	// no url-- won't do anything
	}

	ns->SetUrl(url);
	ns->SetTitle(title);
	ns->SetActive(active);
	ns->SetVisibility(visible);
	
	if (r)
		ns->SetResource(r);

	if (addwhat == "rsl")
	{
		if (!r)
		{
			delete ns;
			return;	// rsl but no resource, so just forget it.
		}
		ns->SetResource(r);
		ns->SetType(type_rsl);
	}
	else	// all other surl types have a file associated with them
	{
		file_surl *fs = (file_surl *) ns;
		if (addwhat == "file")
		{
			fs->SetType(type_FILE);
			fs->SetFile(path);
		}
		else if (addwhat == "directory")
		{
			if (allowlinks)
				ns->SetType(type_DirectoryLink);
			else
				ns->SetType(type_Directory);
			fs->SetFile(index);
		}
		else if (addwhat == "cgi")
		{
			fs->SetType(type_CGIExternal);
			fs->SetFile(cgi);
		}
		else if (addwhat == "location")
		{
			if (location.length() <= 0)
				return;	// location surl with no "location" == ?
			fs->SetType(type_Location);
			fs->SetFile(location);
		}
	}
	
	surls.append(ns);
#ifdef DEBUG
	cout << '\n';
#endif
}


void R_Surlist::print(void)
{
	
}

// Find a SURL by url name
surl *R_Surlist::operator()(String& aurl)
{
Pix temp = surls.first();
surl *s = NULL;

#ifdef DEBUG
	cout << "Surlist: looking for surl " << aurl << ": ";
#endif

	// go through the list of surls
	for(; temp; surls.next(temp))
	{
		s = surls(temp);
		if (s && s->MatchesUrl(aurl))	// ask the surl if it matches
		{
#ifdef DEBUG
			cout << "found.\n";
#endif
			return s;
		}
	}
#ifdef DEBUG
	cout << "not found.\n";
#endif
	return NULL;
}
