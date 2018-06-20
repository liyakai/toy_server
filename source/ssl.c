#include "toyServer_private.h"
#include "toyServerErrcode.h"
#include <openssl/ssl.h>
#include <openssl/err.h>

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
	if(SSL_CTX_use_certificate_file(ctx, CertFile, SSL_FILETYPE_PEM) <= 0)
	{
		ERR_print_errors_fp(stderr);
		return SEC_CONN_READ_CERTFILE_FAIL;
	}
	return SEC_OK;
}
