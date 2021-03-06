#include "toyServer.h"
#include <stdio.h>

int clientLog(void *hLog, int nLevel, const char *szMessage);

int main(int argc, char** argv)
{
	int rv = 0;
	TSecCliSetting setting = {
        1,                                 // whether to use ssl
        1,                                 // whether to verify peer cert
        "../../../certs/client.crt",       // Cert Path
		"../../../certs/client.key",         // Key Path
        "../../../certs/rootca.crt",        // ca Path
        };
    void* phSession = NULL;
	
    toySetLogCB("some logs about client", clientLog);
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


int clientLog(void *hLog, int nLevel, const char *szMessage)
{
    fprintf(stderr, "%s_level:%d_%s\n",(char*)hLog, nLevel, szMessage);
    return 0;
};