
#include "mbedtls/config.h"
#include "mbedtls/platform.h"

#include "mbedtls/net.h"
#include "mbedtls/debug.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/error.h"
#include "mbedtls/certs.h"

#include "esp_common.h"

static const char default_cas_certificate[] ICACHE_RODATA_ATTR STORE_ATTR = {
  0x30, 0x82, 0x05, 0x38, 0x30, 0x82, 0x04, 0x20, 0xa0, 0x03, 0x02, 0x01,
  0x02, 0x02, 0x10, 0x51, 0x3f, 0xb9, 0x74, 0x38, 0x70, 0xb7, 0x34, 0x40,
  0x41, 0x8d, 0x30, 0x93, 0x06, 0x99, 0xff, 0x30, 0x0d, 0x06, 0x09, 0x2a,
  0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x0b, 0x05, 0x00, 0x30, 0x81,
  0xca, 0x31, 0x0b, 0x30, 0x09, 0x06, 0x03, 0x55, 0x04, 0x06, 0x13, 0x02,
  0x55, 0x53, 0x31, 0x17, 0x30, 0x15, 0x06, 0x03, 0x55, 0x04, 0x0a, 0x13,
  0x0e, 0x56, 0x65, 0x72, 0x69, 0x53, 0x69, 0x67, 0x6e, 0x2c, 0x20, 0x49,
  0x6e, 0x63, 0x2e, 0x31, 0x1f, 0x30, 0x1d, 0x06, 0x03, 0x55, 0x04, 0x0b,
  0x13, 0x16, 0x56, 0x65, 0x72, 0x69, 0x53, 0x69, 0x67, 0x6e, 0x20, 0x54,
  0x72, 0x75, 0x73, 0x74, 0x20, 0x4e, 0x65, 0x74, 0x77, 0x6f, 0x72, 0x6b,
  0x31, 0x3a, 0x30, 0x38, 0x06, 0x03, 0x55, 0x04, 0x0b, 0x13, 0x31, 0x28,
  0x63, 0x29, 0x20, 0x32, 0x30, 0x30, 0x36, 0x20, 0x56, 0x65, 0x72, 0x69,
  0x53, 0x69, 0x67, 0x6e, 0x2c, 0x20, 0x49, 0x6e, 0x63, 0x2e, 0x20, 0x2d,
  0x20, 0x46, 0x6f, 0x72, 0x20, 0x61, 0x75, 0x74, 0x68, 0x6f, 0x72, 0x69,
  0x7a, 0x65, 0x64, 0x20, 0x75, 0x73, 0x65, 0x20, 0x6f, 0x6e, 0x6c, 0x79,
  0x31, 0x45, 0x30, 0x43, 0x06, 0x03, 0x55, 0x04, 0x03, 0x13, 0x3c, 0x56,
  0x65, 0x72, 0x69, 0x53, 0x69, 0x67, 0x6e, 0x20, 0x43, 0x6c, 0x61, 0x73,
  0x73, 0x20, 0x33, 0x20, 0x50, 0x75, 0x62, 0x6c, 0x69, 0x63, 0x20, 0x50,
  0x72, 0x69, 0x6d, 0x61, 0x72, 0x79, 0x20, 0x43, 0x65, 0x72, 0x74, 0x69,
  0x66, 0x69, 0x63, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x20, 0x41, 0x75, 0x74,
  0x68, 0x6f, 0x72, 0x69, 0x74, 0x79, 0x20, 0x2d, 0x20, 0x47, 0x35, 0x30,
  0x1e, 0x17, 0x0d, 0x31, 0x33, 0x31, 0x30, 0x33, 0x31, 0x30, 0x30, 0x30,
  0x30, 0x30, 0x30, 0x5a, 0x17, 0x0d, 0x32, 0x33, 0x31, 0x30, 0x33, 0x30,
  0x32, 0x33, 0x35, 0x39, 0x35, 0x39, 0x5a, 0x30, 0x7e, 0x31, 0x0b, 0x30,
  0x09, 0x06, 0x03, 0x55, 0x04, 0x06, 0x13, 0x02, 0x55, 0x53, 0x31, 0x1d,
  0x30, 0x1b, 0x06, 0x03, 0x55, 0x04, 0x0a, 0x13, 0x14, 0x53, 0x79, 0x6d,
  0x61, 0x6e, 0x74, 0x65, 0x63, 0x20, 0x43, 0x6f, 0x72, 0x70, 0x6f, 0x72,
  0x61, 0x74, 0x69, 0x6f, 0x6e, 0x31, 0x1f, 0x30, 0x1d, 0x06, 0x03, 0x55,
  0x04, 0x0b, 0x13, 0x16, 0x53, 0x79, 0x6d, 0x61, 0x6e, 0x74, 0x65, 0x63,
  0x20, 0x54, 0x72, 0x75, 0x73, 0x74, 0x20, 0x4e, 0x65, 0x74, 0x77, 0x6f,
  0x72, 0x6b, 0x31, 0x2f, 0x30, 0x2d, 0x06, 0x03, 0x55, 0x04, 0x03, 0x13,
  0x26, 0x53, 0x79, 0x6d, 0x61, 0x6e, 0x74, 0x65, 0x63, 0x20, 0x43, 0x6c,
  0x61, 0x73, 0x73, 0x20, 0x33, 0x20, 0x53, 0x65, 0x63, 0x75, 0x72, 0x65,
  0x20, 0x53, 0x65, 0x72, 0x76, 0x65, 0x72, 0x20, 0x43, 0x41, 0x20, 0x2d,
  0x20, 0x47, 0x34, 0x30, 0x82, 0x01, 0x22, 0x30, 0x0d, 0x06, 0x09, 0x2a,
  0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x01, 0x05, 0x00, 0x03, 0x82,
  0x01, 0x0f, 0x00, 0x30, 0x82, 0x01, 0x0a, 0x02, 0x82, 0x01, 0x01, 0x00,
  0xb2, 0xd8, 0x05, 0xca, 0x1c, 0x74, 0x2d, 0xb5, 0x17, 0x56, 0x39, 0xc5,
  0x4a, 0x52, 0x09, 0x96, 0xe8, 0x4b, 0xd8, 0x0c, 0xf1, 0x68, 0x9f, 0x9a,
  0x42, 0x28, 0x62, 0xc3, 0xa5, 0x30, 0x53, 0x7e, 0x55, 0x11, 0x82, 0x5b,
  0x03, 0x7a, 0x0d, 0x2f, 0xe1, 0x79, 0x04, 0xc9, 0xb4, 0x96, 0x77, 0x19,
  0x81, 0x01, 0x94, 0x59, 0xf9, 0xbc, 0xf7, 0x7a, 0x99, 0x27, 0x82, 0x2d,
  0xb7, 0x83, 0xdd, 0x5a, 0x27, 0x7f, 0xb2, 0x03, 0x7a, 0x9c, 0x53, 0x25,
  0xe9, 0x48, 0x1f, 0x46, 0x4f, 0xc8, 0x9d, 0x29, 0xf8, 0xbe, 0x79, 0x56,
  0xf6, 0xf7, 0xfd, 0xd9, 0x3a, 0x68, 0xda, 0x8b, 0x4b, 0x82, 0x33, 0x41,
  0x12, 0xc3, 0xc8, 0x3c, 0xcc, 0xd6, 0x96, 0x7a, 0x84, 0x21, 0x1a, 0x22,
  0x04, 0x03, 0x27, 0x17, 0x8b, 0x1c, 0x68, 0x61, 0x93, 0x0f, 0x0e, 0x51,
  0x80, 0x33, 0x1d, 0xb4, 0xb5, 0xce, 0xeb, 0x7e, 0xd0, 0x62, 0xac, 0xee,
  0xb3, 0x7b, 0x01, 0x74, 0xef, 0x69, 0x35, 0xeb, 0xca, 0xd5, 0x3d, 0xa9,
  0xee, 0x97, 0x98, 0xca, 0x8d, 0xaa, 0x44, 0x0e, 0x25, 0x99, 0x4a, 0x15,
  0x96, 0xa4, 0xce, 0x6d, 0x02, 0x54, 0x1f, 0x2a, 0x6a, 0x26, 0xe2, 0x06,
  0x3a, 0x63, 0x48, 0xac, 0xb4, 0x4c, 0xd1, 0x75, 0x93, 0x50, 0xff, 0x13,
  0x2f, 0xd6, 0xda, 0xe1, 0xc6, 0x18, 0xf5, 0x9f, 0xc9, 0x25, 0x5d, 0xf3,
  0x00, 0x3a, 0xde, 0x26, 0x4d, 0xb4, 0x29, 0x09, 0xcd, 0x0f, 0x3d, 0x23,
  0x6f, 0x16, 0x4a, 0x81, 0x16, 0xfb, 0xf2, 0x83, 0x10, 0xc3, 0xb8, 0xd6,
  0xd8, 0x55, 0x32, 0x3d, 0xf1, 0xbd, 0x0f, 0xbd, 0x8c, 0x52, 0x95, 0x4a,
  0x16, 0x97, 0x7a, 0x52, 0x21, 0x63, 0x75, 0x2f, 0x16, 0xf9, 0xc4, 0x66,
  0xbe, 0xf5, 0xb5, 0x09, 0xd8, 0xff, 0x27, 0x00, 0xcd, 0x44, 0x7c, 0x6f,
  0x4b, 0x3f, 0xb0, 0xf7, 0x02, 0x03, 0x01, 0x00, 0x01, 0xa3, 0x82, 0x01,
  0x63, 0x30, 0x82, 0x01, 0x5f, 0x30, 0x12, 0x06, 0x03, 0x55, 0x1d, 0x13,
  0x01, 0x01, 0xff, 0x04, 0x08, 0x30, 0x06, 0x01, 0x01, 0xff, 0x02, 0x01,
  0x00, 0x30, 0x30, 0x06, 0x03, 0x55, 0x1d, 0x1f, 0x04, 0x29, 0x30, 0x27,
  0x30, 0x25, 0xa0, 0x23, 0xa0, 0x21, 0x86, 0x1f, 0x68, 0x74, 0x74, 0x70,
  0x3a, 0x2f, 0x2f, 0x73, 0x31, 0x2e, 0x73, 0x79, 0x6d, 0x63, 0x62, 0x2e,
  0x63, 0x6f, 0x6d, 0x2f, 0x70, 0x63, 0x61, 0x33, 0x2d, 0x67, 0x35, 0x2e,
  0x63, 0x72, 0x6c, 0x30, 0x0e, 0x06, 0x03, 0x55, 0x1d, 0x0f, 0x01, 0x01,
  0xff, 0x04, 0x04, 0x03, 0x02, 0x01, 0x06, 0x30, 0x2f, 0x06, 0x08, 0x2b,
  0x06, 0x01, 0x05, 0x05, 0x07, 0x01, 0x01, 0x04, 0x23, 0x30, 0x21, 0x30,
  0x1f, 0x06, 0x08, 0x2b, 0x06, 0x01, 0x05, 0x05, 0x07, 0x30, 0x01, 0x86,
  0x13, 0x68, 0x74, 0x74, 0x70, 0x3a, 0x2f, 0x2f, 0x73, 0x32, 0x2e, 0x73,
  0x79, 0x6d, 0x63, 0x62, 0x2e, 0x63, 0x6f, 0x6d, 0x30, 0x6b, 0x06, 0x03,
  0x55, 0x1d, 0x20, 0x04, 0x64, 0x30, 0x62, 0x30, 0x60, 0x06, 0x0a, 0x60,
  0x86, 0x48, 0x01, 0x86, 0xf8, 0x45, 0x01, 0x07, 0x36, 0x30, 0x52, 0x30,
  0x26, 0x06, 0x08, 0x2b, 0x06, 0x01, 0x05, 0x05, 0x07, 0x02, 0x01, 0x16,
  0x1a, 0x68, 0x74, 0x74, 0x70, 0x3a, 0x2f, 0x2f, 0x77, 0x77, 0x77, 0x2e,
  0x73, 0x79, 0x6d, 0x61, 0x75, 0x74, 0x68, 0x2e, 0x63, 0x6f, 0x6d, 0x2f,
  0x63, 0x70, 0x73, 0x30, 0x28, 0x06, 0x08, 0x2b, 0x06, 0x01, 0x05, 0x05,
  0x07, 0x02, 0x02, 0x30, 0x1c, 0x1a, 0x1a, 0x68, 0x74, 0x74, 0x70, 0x3a,
  0x2f, 0x2f, 0x77, 0x77, 0x77, 0x2e, 0x73, 0x79, 0x6d, 0x61, 0x75, 0x74,
  0x68, 0x2e, 0x63, 0x6f, 0x6d, 0x2f, 0x72, 0x70, 0x61, 0x30, 0x29, 0x06,
  0x03, 0x55, 0x1d, 0x11, 0x04, 0x22, 0x30, 0x20, 0xa4, 0x1e, 0x30, 0x1c,
  0x31, 0x1a, 0x30, 0x18, 0x06, 0x03, 0x55, 0x04, 0x03, 0x13, 0x11, 0x53,
  0x79, 0x6d, 0x61, 0x6e, 0x74, 0x65, 0x63, 0x50, 0x4b, 0x49, 0x2d, 0x31,
  0x2d, 0x35, 0x33, 0x34, 0x30, 0x1d, 0x06, 0x03, 0x55, 0x1d, 0x0e, 0x04,
  0x16, 0x04, 0x14, 0x5f, 0x60, 0xcf, 0x61, 0x90, 0x55, 0xdf, 0x84, 0x43,
  0x14, 0x8a, 0x60, 0x2a, 0xb2, 0xf5, 0x7a, 0xf4, 0x43, 0x18, 0xef, 0x30,
  0x1f, 0x06, 0x03, 0x55, 0x1d, 0x23, 0x04, 0x18, 0x30, 0x16, 0x80, 0x14,
  0x7f, 0xd3, 0x65, 0xa7, 0xc2, 0xdd, 0xec, 0xbb, 0xf0, 0x30, 0x09, 0xf3,
  0x43, 0x39, 0xfa, 0x02, 0xaf, 0x33, 0x31, 0x33, 0x30, 0x0d, 0x06, 0x09,
  0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x0b, 0x05, 0x00, 0x03,
  0x82, 0x01, 0x01, 0x00, 0x5e, 0x94, 0x56, 0x49, 0xdd, 0x8e, 0x2d, 0x65,
  0xf5, 0xc1, 0x36, 0x51, 0xb6, 0x03, 0xe3, 0xda, 0x9e, 0x73, 0x19, 0xf2,
  0x1f, 0x59, 0xab, 0x58, 0x7e, 0x6c, 0x26, 0x05, 0x2c, 0xfa, 0x81, 0xd7,
  0x5c, 0x23, 0x17, 0x22, 0x2c, 0x37, 0x93, 0xf7, 0x86, 0xec, 0x85, 0xe6,
  0xb0, 0xa3, 0xfd, 0x1f, 0xe2, 0x32, 0xa8, 0x45, 0x6f, 0xe1, 0xd9, 0xfb,
  0xb9, 0xaf, 0xd2, 0x70, 0xa0, 0x32, 0x42, 0x65, 0xbf, 0x84, 0xfe, 0x16,
  0x2a, 0x8f, 0x3f, 0xc5, 0xa6, 0xd6, 0xa3, 0x93, 0x7d, 0x43, 0xe9, 0x74,
  0x21, 0x91, 0x35, 0x28, 0xf4, 0x63, 0xe9, 0x2e, 0xed, 0xf7, 0xf5, 0x5c,
  0x7f, 0x4b, 0x9a, 0xb5, 0x20, 0xe9, 0x0a, 0xbd, 0xe0, 0x45, 0x10, 0x0c,
  0x14, 0x94, 0x9a, 0x5d, 0xa5, 0xe3, 0x4b, 0x91, 0xe8, 0x24, 0x9b, 0x46,
  0x40, 0x65, 0xf4, 0x22, 0x72, 0xcd, 0x99, 0xf8, 0x88, 0x11, 0xf5, 0xf3,
  0x7f, 0xe6, 0x33, 0x82, 0xe6, 0xa8, 0xc5, 0x7e, 0xfe, 0xd0, 0x08, 0xe2,
  0x25, 0x58, 0x08, 0x71, 0x68, 0xe6, 0xcd, 0xa2, 0xe6, 0x14, 0xde, 0x4e,
  0x52, 0x24, 0x2d, 0xfd, 0xe5, 0x79, 0x13, 0x53, 0xe7, 0x5e, 0x2f, 0x2d,
  0x4d, 0x1b, 0x6d, 0x40, 0x15, 0x52, 0x2b, 0xf7, 0x87, 0x89, 0x78, 0x12,
  0x81, 0x6e, 0xd9, 0x4d, 0xaa, 0x2d, 0x78, 0xd4, 0xc2, 0x2c, 0x3d, 0x08,
  0x5f, 0x87, 0x91, 0x9e, 0x1f, 0x0e, 0xb0, 0xde, 0x30, 0x52, 0x64, 0x86,
  0x89, 0xaa, 0x9d, 0x66, 0x9c, 0x0e, 0x76, 0x0c, 0x80, 0xf2, 0x74, 0xd8,
  0x2a, 0xf8, 0xb8, 0x3a, 0xce, 0xd7, 0xd6, 0x0f, 0x11, 0xbe, 0x6b, 0xab,
  0x14, 0xf5, 0xbd, 0x41, 0xa0, 0x22, 0x63, 0x89, 0xf1, 0xba, 0x0f, 0x6f,
  0x29, 0x63, 0x66, 0x2d, 0x3f, 0xac, 0x8c, 0x72, 0xc5, 0xfb, 0xc7, 0xe4,
  0xd4, 0x0f, 0xf2, 0x3b, 0x4f, 0x8c, 0x29, 0xc7
};
unsigned int default_cas_certificate_len = 1340;

int mbedtls_library_init(const char *host, const char *port)
{
    int ret, len;
    mbedtls_net_context server_fd;
    uint32_t flags;
    unsigned char buf[256];
    const char *pers = "ssl_client1";

	printf("mbedtls_library_init(): Connecting to host \"%s\", port \"\%s\"\n", host, port);

    mbedtls_entropy_context *entropy = (mbedtls_entropy_context *)zalloc(sizeof(mbedtls_entropy_context));
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_ssl_context *ssl = (mbedtls_ssl_context *)zalloc(sizeof(mbedtls_ssl_context));
    mbedtls_ssl_config *conf = (mbedtls_ssl_config *)zalloc(sizeof(mbedtls_ssl_config));
	
    mbedtls_x509_crt *cacert = (mbedtls_x509_crt *)zalloc(sizeof(mbedtls_x509_crt));
	mbedtls_x509_crt *clicert = (mbedtls_x509_crt *)zalloc(sizeof(mbedtls_x509_crt));
	mbedtls_pk_context *pkey = (mbedtls_pk_context *)zalloc(sizeof(mbedtls_pk_context));

	/*
     * 0. Initialize the RNG and the session data
     */

    mbedtls_printf( "\n  . Seeding the random number generator..." );

    mbedtls_entropy_init( entropy );
    if( ( ret = mbedtls_ctr_drbg_seed( &ctr_drbg, mbedtls_entropy_func, entropy,
                               (const unsigned char *) pers,
                               strlen( pers ) ) ) != 0 )
    {
        mbedtls_printf( " failed\n  ! mbedtls_ctr_drbg_seed returned %d\n", ret );
        goto exit;
    }

    mbedtls_printf( " ok\n" );







	






exit:

#ifdef MBEDTLS_ERROR_C
    if( ret != 0 )
    {
        char error_buf[100];
        mbedtls_strerror( ret, error_buf, 100 );
        mbedtls_printf("Last error was: %d - %s\n\n", ret, error_buf );
    }
#endif

//    mbedtls_net_free( &server_fd );

    mbedtls_x509_crt_free( cacert );
	mbedtls_x509_crt_free( clicert );
	mbedtls_pk_free( pkey );
    mbedtls_ssl_free( ssl );
    mbedtls_ssl_config_free( conf );
//    mbedtls_ctr_drbg_free( &ctr_drbg );
    mbedtls_entropy_free( entropy );
	free(ssl);
	free(cacert);
	free(clicert);
	free(pkey);
	free(entropy);
	free(conf);

    return( ret );
}
