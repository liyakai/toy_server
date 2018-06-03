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

TOY_SERVER_API int toyClient(int argc, char** argv);
TOY_SERVER_API int toyServer(int argc, char** argv);

#ifdef  __cplusplus
}
#endif
#endif    /** _TOY_SERVER_H_ */
