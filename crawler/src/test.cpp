#include <stdio.h>
#include <stdlib.h>
#include <iostream>

using namespace std;


class Test
{
public:
    void bam() { cout << __FUNCTION__ << endl; }
};

int main()
{
	Test* pTest = new Test();
	pTest->bam();
	exit(0);
}
