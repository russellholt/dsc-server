#ifndef _STRING_PAIR_H_
#define _STRING_PAIR_H_ 1

#include <String.h>

class stringpair {
protected:
	String l, r;
public:
	stringpair(void) { }

	stringpair(char *left, char *right)
	{ l = left; r = right; }

	stringpair(String& left, String& right)
	{ l = left; r = right; }

	inline void SetLeft(String &left)
		{ l = left; }
	inline void SetLeft(char *str)
		{ l = str; }

	inline void SetRight(String &right)
		{ r = right; }
	inline void SetRight(char *str)
		{ r = str; }

	inline String& left(void)
		{ return l; }
	inline String& right(void)
		{ return r; }
	
	// double set
	inline void operator() (String& left, String& right)
	{
			l = left;
			r = right;
	}

	inline void operator() (char *left, char *right)
	{
			l = left;
			r = right;
	}
};

#endif
