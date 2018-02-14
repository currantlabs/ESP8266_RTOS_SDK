#ifndef __NOPOLL_MBEDTLS_SHIM_H__
#define __NOPOLL_MBEDTLS_SHIM_H__

#include "mbedtls/ssl.h"

int mbedtls_library_init(mbedtls_ssl_context **ssl, const char *host, const char *port);


#endif // __NOPOLL_H__
