// cctest.cc
// compile with:
//		g++ cctest.cc -lg++ -o cctest
#include <String.h>
#include <SLList.h>
#include <stream.h>

void printlist(SLList<String> &x);

main()
{
SLList<String> mylist;
String a,b;

	a = "hello";
	b = "this is side 2!";

	mylist.append(a);
	mylist.append(b);

SLList<String> list2;

	list2 = mylist;

	cout << "list 1 is:\n";
	printlist(mylist);
	cout << "\nlist 2 is:\n";
	printlist(list2);
	cout << "\n\n";
}


void printlist(SLList<String> &x)
{
Pix temp = x.first();
	for(;temp; x.next(temp))
		cout << x(temp) << '\n';
}
