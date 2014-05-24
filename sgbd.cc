#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "sgbd.h"
#include "Shell.h"

int main()
{
	string *dir = new string("./");

	Shell *teste = new Shell(dir);

	delete teste;
	delete dir;

	return 0;
}
