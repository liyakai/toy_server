#include "toyServer.h"
#include <stdio.h>

int main(int argc, char** argv)
{
	fprintf(stderr, "run clientTest.\n");
	toyClient(argc, argv);
	return 0;
}
