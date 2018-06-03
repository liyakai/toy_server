#include "toyServer.h"
#include <stdio.h>

int main(int argc, char** argv)
{
	fprintf(stderr, "running Server...\n");
    toyServer(argc, argv);
	return 0;
}
