#ifndef TOY_SERVER_ERRCODE
#define TOY_SERVER_ERRCODE

// the error code of TOYSERVER


#define SEC_OK                                     0
#define SEC_BASENO                                 0x00000000
#define SEC_UNKNOW                                 (SEC_BASENO + 0x0FFFFFFF)


/**************************************************
*        ToyServer(0x1010000~0x1010FFF)
**************************************************/

#define SEC_TOYSERVER_LIB_BASENO                   0x1010000 
#define SEC_TOYSERVER_LIB_ENDNO                    0x1010FFF

// API: 0x000~0x0ff
#define SEC_TOYSERVER_API_BASENO                   (SEC_TOYSERVER_LIB_BASENO + 0x00)
#define SEC_API_LOG_INIT_FAIL                      (SEC_TOYSERVER_API_BASENO + 0x01)


#endif  // TOY_SERVER_ERRCODE

