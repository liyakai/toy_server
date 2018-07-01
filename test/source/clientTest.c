#include "toyServer.h"
#include <stdio.h>

int main(int argc, char** argv)
{
	int rv = 0;
	TSecCliSetting setting = {1};
    void* phSession = NULL;
	
	fprintf(stderr, "run clientTest.\n");
	rv = toyClientSessionCreate(&setting, &phSession);
    if(rv)
    {
        fprintf(stderr, "toyClientSessionCreate failed. rv=%x\n",rv);
    }
	fprintf(stderr, "before toyClient.\n");
    rv = toyClient(phSession, argc, argv);
	if(rv)
	{
		fprintf(stderr, "toyClient failed.rv = %x\n",rv);
	}
    rv = toyClientSessionDestroy(phSession);
    if(rv)
    {
        fprintf(stderr, "toyClientSessionDestroy failed.\n");
    }

	return 0;
}
