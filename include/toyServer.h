#ifndef _TOY_SERVER_H_
#define _TOY_SERVER_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef WIN32
#ifdef TOYSERVERDLL_EXPORTS
#define TOY_SERVER_API __declspec(dllexport)
#else
#define TOY_SERVER_API __declspec(dllimport)
#endif
#else
#define TOY_SERVER_API __attribute__ ((visibility("default")))
#endif

typedef struct tagSecSerSetting
{
	int bUseSSL;
	const char* pszServerCert;
	const char* pszServerKey;
} TSecSerSetting;

TOY_SERVER_API int toyServerCreate(TSecSerSetting *tSetting, void** phInstance);
TOY_SERVER_API int toyServerDestroy(void* phInstance);
TOY_SERVER_API int toyServer(void* phInstance, int argc, char** argv);

typedef struct tagSecCliSetting
{
	int bUseSSL;
} TSecCliSetting;

TOY_SERVER_API int toyClientSessionCreate(TSecCliSetting *tSetting, void** phSession);
TOY_SERVER_API int toyClientSessionDestroy(void* phSession);
TOY_SERVER_API int toyClient(void* phSession, int argc, char** argv);
#ifdef  __cplusplus
}
#endif
#endif    /** _TOY_SERVER_H_ */
