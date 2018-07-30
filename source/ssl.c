#include "toyServer_private.h"
#include <string.h>


#define LISTENQ 1024

// server
int openListener(int port)
{
	int sd;
	struct sockaddr_in addr;
	sd = socket(AF_INET, SOCK_STREAM, 0);
	memset(&addr, 0,  sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr=INADDR_ANY;
	if(bind(sd, (struct sockaddr*)&addr, sizeof(addr)) != 0)
	{
		LOG_ERROR("can't bind port\n");
		return SEC_CONN_SOCKET_BINDPORT_FAILED;
	}
	if(listen(sd, LISTENQ) != 0)
	{
		LOG_ERROR("Can't configure listening port.\n");
		return SEC_CONN_SOCKET_LISTEN_FAILED;
	}
	return sd;
}


// server
SSL_CTX* initServerSslCtx(void)
{
	SSL_CTX * ctx = NULL;
    const SSL_METHOD *method = NULL;

	SSL_library_init();
	OpenSSL_add_all_algorithms();
	SSL_load_error_strings();
	ERR_load_ERR_strings();
    ERR_load_crypto_strings();
	method = SSLv23_server_method();
	ctx = SSL_CTX_new(method);
	if(ctx == NULL)
	{
		ERR_print_errors_fp(stderr);
		return NULL;
	}
	return ctx;
}

// server
int loadCertFile(SSL_CTX* ctx, const char* CertFile, const char* KeyFile, const char* caFile)
{
	if(!ctx)
	{
		LOG_ERROR("loadCertFile -->> SEC_API_PARAMS_HASNULL\n");
		return SEC_API_PARAMS_HASNULL;
	}
	// load cert
	if(CertFile && SSL_CTX_use_certificate_file(ctx, CertFile, SSL_FILETYPE_PEM) <= 0)
	{
		ERR_print_errors_fp(stderr);
		LOG_ERROR("loadCertFile -->> load CertFile failed\n");
		return SEC_CONN_READ_CERTFILE_FAIL;
	}

	// load private key
	if(KeyFile && SSL_CTX_use_PrivateKey_file(ctx,KeyFile, SSL_FILETYPE_PEM) <= 0)
	{
		ERR_print_errors_fp(stderr);
		LOG_ERROR("loadCertFile -->> load KeyFile failed\n");
		return SEC_CONN_READ_PRIVATEKEY_FAIL;

	}

	// check private key
	if(CertFile && KeyFile && !SSL_CTX_check_private_key(ctx))
	{
		LOG_ERROR("Private key does not match the cert\n");
		return SEC_CONN_PRIVATEKEY_NOTMATCH_CERT;
	}

    // load ca file
	if(caFile && !SSL_CTX_load_verify_locations(ctx, caFile, 0))
	{
		LOG_ERROR("loadCertFile -->> load cafile failed\n");
		return SEC_CONN_READ_CAFILE_FAIL;
	}
	LOG_INFO("loadCertFile -->> LoadCertificates Compleate Successfully...\n");
    return SEC_OK;
}

// for common usr
int showCerts(SSL* ssl)
{
	X509 *cert;
	char *line;
	cert = SSL_get_peer_certificate(ssl);
	if(cert)
	{
		LOG_INFO("Peer certificates:\n");
		line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
		LOG_INFO("SUbject:%s\n", line);
		line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
		LOG_INFO("Issuer:%s\n", line);
		free(line);
		X509_free(cert);
	} else 
	{
		LOG_ERROR("No certificates.\n");
	}
    return SEC_OK;
}



// client
int openConnection(const char* ipAddr, int port)
{
	int sd;
	struct sockaddr_in addr;
	LOG_INFO("openConnection -->> ipAddr:%s; port:%d\n", ipAddr, port);
	sd = socket(AF_INET, SOCK_STREAM, 0);
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
    if(inet_pton(AF_INET, ipAddr, &addr.sin_addr) <= 0)
	{
		LOG_ERROR("inet_pton error for %s", ipAddr);
		return SEC_CONN_CLIENT_INETPTON_FAIL;
	}	
	
	
	if(connect(sd, (struct sockaddr*)&addr, sizeof(addr)) != 0)
	{
		close(sd);
		LOG_ERROR("client connect failed.\n");
		return SEC_CONN_CLIENT_CONNECT_FAIL;
	}
	return sd;
}

SSL_CTX* initCliSslCtx(void)
{
    const	SSL_METHOD *method;
	SSL_CTX *ctx;
	OpenSSL_add_all_algorithms();
	SSL_load_error_strings();
	method = SSLv23_client_method();
	ctx = SSL_CTX_new(method);
	if(ctx == NULL)
	{
		// ERR_print_errors_fp(stderr);
		LOG_ERROR("SSL_CTX_new failed.\n");
	    //	return SEC_CONN_CLIENT_NEW_SSLCTX_FAIL;
		return NULL;
	}
	return ctx;
}

SSL* getCliSsl(void*phSession, int sockfd)
{
	int rv = 0;
	SSL_CTX *ssl_ctx;
    SSL *ssl;
	char recvline[1];
    ssl_ctx = initCliSslCtx();
    if(!ssl_ctx)
    {
        LOG_ERROR("getCliSsl -->> initCliSslCtx failed.\n");
		return NULL;
    }
	LOG_INFO("Client Cert Path:\n%s\n",((tCliSession*)phSession) -> pszClientCert);
	LOG_INFO("Client Key Path:\n%s\n",((tCliSession*)phSession) -> pszClientKey);
	LOG_INFO("Client CA Path:\n%s\n",((tCliSession*)phSession) -> pszClientCA);
	rv  = loadCertFile(ssl_ctx, ((tCliSession*)phSession) -> pszClientCert, ((tCliSession*)phSession) -> pszClientKey, ((tCliSession*)phSession) -> pszClientCA);
	if(rv)
	{
		LOG_ERROR("getCliSsl -->> loadCertFile.\n");
		return NULL;
	}
    ssl = SSL_new(ssl_ctx);    // create new SSL connection state
    if(!ssl)
    {
        LOG_ERROR("getCliSsl -->> SSL_new failed.\n");
		return NULL;
    }
    SSL_set_connect_state(ssl);
    SSL_set_fd(ssl, sockfd);   // attache socet descriptor
    if(SSL_connect(ssl)  != 1)   // perform the connection
    {
        LOG_ERROR("getCliSsl -->> SSL_connect failed\n");
        return NULL;
    } else
    {
        LOG_INFO("getCliSsl -->> Connected with %s encryption\n", SSL_get_cipher(ssl));
		showCerts(ssl);
    }
    SSL_read(ssl, recvline, 0);
    SSL_CTX_free(ssl_ctx);
	return ssl;
}
