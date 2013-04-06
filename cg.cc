#include <String.h>
#include <ctype.h>

String unescape_hex(String h);

Regex esc_hex("%[0-9a-fA-F][0-9a-fA-F]");	// percent followed by two hex digits

main()
{
String a = "something=%26%27%28something+%2fand+%2fstuff&name=my+name&email=my+e%2fmail&color=my+fav+color";
	int i = a.freq("&"), i2;

	cout << "a  = " << a << '\n';

	String list[i+1];
	i2 = split(a, list, i, String("&"));
	
	String left, right;
	for (i = 0; i<i2; i++)
	{
		list[i].gsub("+", " ");
		left = unescape_hex(list[i].before("="));
		right = unescape_hex(list[i].after("="));
		cout << '(' << left << ", " << right << ")\n";
	}

}

String unescape_hex(String h)
{
String hexdig = "0123456789abcdef";
int i, li=0;
String uesc;
char c, lsb, msb;

	while ((i = h.index(esc_hex, li)) >= 0)
	{
		uesc = h.at(i, 3);	// %xx
		msb = tolower(uesc[1]);
		lsb = tolower(uesc[2]);
		if (msb > 0 && lsb > 0) {
			c = (char) (hexdig.index(msb) * 16 + hexdig.index(lsb));
			//	h.gsub(uesc, c);
			h.at(i,3) = c;
			li = i;
		}
	}
	return h;
}