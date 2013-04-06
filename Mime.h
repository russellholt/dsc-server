#ifndef _MIME_H_
#define _MIME_H_ 1

#include <SLList.h>
#include "stringpair.h"

class Mime {
	String configFileName;
	SLList<stringpair> list;

public:
	Mime(char *filename);
	void ReadConfigFile(void);
	int GetType(String& extension, String& content_type);
	int GetExtension(String& content_type, String& extension);
	void CreateMinimalList(void);

	int AddType(String& extension, String& content_type);

	inline void QuickAddType(String& extension, String& content_type)
	{
		stringpair e;
		e(extension, content_type);
		list.append(e);
	}

	inline void QuickAddType(char *extension, char *content_type)
	{
		stringpair e;
		e(extension, content_type);
		list.append(e);
	}

	inline SLList<stringpair>& GetList(void)
		{	return list;	}
};

#endif
