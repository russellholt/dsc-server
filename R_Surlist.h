
#ifndef R_SURLIST_H_
#define R_SURLIST_H_

#include <SLList.h>
#include <String.h>
#include "../crb/resource.h"
#include "surl.h"

class R_Surlist : public resource {
	SLList<surl *> surls;
	Pix where;
	void Init(void) { where = NULL; class_name = "Surlist"; }
public:
	
	R_Surlist(void) { }
	R_Surlist(String &nm) { name = nm; }
	R_Surlist(char *nm) { name = nm; }
	resource *Create(String &nm, resource *table=NULL);

	resource *execute(String& method, SLList<resource *> &args);
	void print(void);

	void Add(SLList<resource *> &args, String &addwhat);

	surl *operator()(String& name);
};

#endif