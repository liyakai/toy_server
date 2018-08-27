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
#define SEC_TOYSERVER_API_BASENO                   (SEC_TOYSERVER_LIB_BASENO + 0x000)
#define SEC_API_LOG_INIT_FAIL                      (SEC_TOYSERVER_API_BASENO + 0x01)
#define SEC_API_PARAMS_HASNULL                     (SEC_TOYSERVER_API_BASENO + 0x02)
#define SEC_API_SSLACCEPT_FAIL                     (SEC_TOYSERVER_API_BASENO + 0x03)
#define SEC_API_SERWRITE_FAIL                      (SEC_TOYSERVER_API_BASENO + 0x04)

// connection : 0x100~0x1ff
#define SEC_TOYSERVER_CONN_BASENO                  (SEC_TOYSERVER_LIB_BASENO + 0x100)
#define SEC_CONN_READ_CERTFILE_FAIL                (SEC_TOYSERVER_CONN_BASENO + 0x01)
#define SEC_CONN_READ_PRIVATEKEY_FAIL              (SEC_TOYSERVER_CONN_BASENO + 0x02)
#define SEC_CONN_PRIVATEKEY_NOTMATCH_CERT          (SEC_TOYSERVER_CONN_BASENO + 0x03)
#define SEC_CONN_SOCKET_BINDPORT_FAILED            (SEC_TOYSERVER_CONN_BASENO + 0x04)
#define SEC_CONN_SOCKET_LISTEN_FAILED              (SEC_TOYSERVER_CONN_BASENO + 0x05)
#define SEC_CONN_CLIENT_INETPTON_FAIL              (SEC_TOYSERVER_CONN_BASENO + 0x06)
#define SEC_CONN_CLIENT_CONNECT_FAIL               (SEC_TOYSERVER_CONN_BASENO + 0x07)
#define SEC_CONN_CLIENT_NEW_SSLCTX_FAIL            (SEC_TOYSERVER_CONN_BASENO + 0x08)
#define SEC_CONN_READ_CAFILE_FAIL                  (SEC_TOYSERVER_CONN_BASENO + 0x09)
#define SEC_CONN_SERVER_READ_FAIL                  (SEC_TOYSERVER_CONN_BASENO + 0x0a)
#define SEC_CONN_CLOSED_BY_CLIENT                  (SEC_TOYSERVER_CONN_BASENO + 0x0b)


#endif  // TOY_SERVER_ERRCODE

