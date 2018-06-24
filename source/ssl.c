#include "toyServer_private.h"
#include "toyServerErrcode.h"
#include <string.h>


#define LISTENQ 1024

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
		fprintf(stderr, "can't bind port\n");
		return SEC_CONN_SOCKET_BINDPORT_FAILED;
	}
	if(listen(sd, LISTENQ) != 0)
	{
		fprintf(stderr, "Can't configure listening port.\n");
		return SEC_CONN_SOCKET_LISTEN_FAILED;
	}
	return sd;
}

SSL_CTX* InitServerSslCtx(void)
{
	SSL_CTX * ctx = NULL;
    const SSL_METHOD *method = NULL;

	SSL_library_init();
	OpenSSL_add_all_algorithms();
	SSL_load_error_strings();
	method = SSLv23_server_method();
	ctx = SSL_CTX_new(method);
	if(ctx == NULL)
	{
		ERR_print_errors_fp(stderr);
		return NULL;
	}
	return ctx;
}

int loadCertFile(SSL_CTX* ctx, const char* CertFile, const char* KeyFile)
{
	// load cert
	if(SSL_CTX_use_certificate_file(ctx, CertFile, SSL_FILETYPE_PEM) <= 0)
	{
		ERR_print_errors_fp(stderr);
		return SEC_CONN_READ_CERTFILE_FAIL;
	}

	// load private key
	if(SSL_CTX_use_PrivateKey_file(ctx,KeyFile, SSL_FILETYPE_PEM) <= 0)
	{
		ERR_print_errors_fp(stderr);
		return SEC_CONN_READ_PRIVATEKEY_FAIL;

	}

	// check private key
	if(!SSL_CTX_check_private_key(ctx))
	{
		fprintf(stderr, "Private key does not match the cert\n");
		return SEC_CONN_PRIVATEKEY_NOTMATCH_CERT;
	}
	fprintf(stderr, "LoadCertificates Compleate Successfully...\n");
    return SEC_OK;
}

int showCerts(SSL* ssl)
{
	X509 *cert;
	char *line;
	cert = SSL_get_peer_certificate(ssl);
	if(cert)
	{
		fprintf(stderr,"Server certificates:\n");
		line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
		fprintf(stderr, "SUbject:%s\n", line);
		line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
		fprintf(stderr, "Issuer:%s\n", line);
		free(line);
		X509_free(cert);
	} else 
	{
		fprintf(stderr, "No certificates.\n");
	}
    return SEC_OK;
}