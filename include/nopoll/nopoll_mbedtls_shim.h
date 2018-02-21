#ifndef __NOPOLL_MBEDTLS_SHIM_H__
#define __NOPOLL_MBEDTLS_SHIM_H__

#include "mbedtls/ssl.h"
#include "mbedtls/net.h"

int mbedtls_library_init(mbedtls_ssl_context *ssl, mbedtls_net_context *server_fd, const char *host, const char *port);


#endif // __NOPOLL_H__
