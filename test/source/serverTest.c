#include "toyServer.h"
#include <stdio.h>


int main(int argc, char** argv)
{
	int rv = 0;
	fprintf(stderr, "running Server...\n");
    TSecSerSetting setting = {
		1,                                // whether to use ssl
		"../../../certs/server.pem",       // Cert Path
		"../../../certs/skey.pem",         // Key Path

	};
	void* phInstance = NULL;
	rv = toyServerCreate(&setting, &phInstance);
	if(rv)
	{
		fprintf(stderr, "toyServerCreate failed.\n");
	}
    rv = toyServer(phInstance, argc, argv);
	if(rv)
	{
		fprintf(stderr, "toyServer failed.\n");
	}
	rv = toyServerDestroy(phInstance);
	if(rv)
    {
        fprintf(stderr, "toyServerDestroy failed.\n");
    }

	return 0;
}
