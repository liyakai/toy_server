#include "toyServer.h"
#include <stdio.h>

int serverLog(void *hLog, int nLevel, const char *szMessage);

int main(int argc, char** argv)
{
	int rv = 0;
	toySetLogCB("some logs about server", serverLog);
	fprintf(stderr, "running Server...\n");
    TSecSerSetting setting = {
		1,                                // whether to use ssl
		1,                                 // whether to verify peer cert
		"../../../certs/server.crt",       // Cert Path
		"../../../certs/server.key",         // Key Path
		"../../../certs/rootca.crt",        // ca Path

	};
	void* phInstance = NULL;
	getSocketOption();
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



int serverLog(void *hLog, int nLevel, const char *szMessage)
{
	fprintf(stderr, "%s_level:%d_%s\n",(char*)hLog, nLevel, szMessage);
	return 0;
};
