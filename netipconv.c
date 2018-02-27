#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>

/*********************************************
 * gcc netipconv.c -o ipconv
*********************************************/

#if defined(__cplusplus)
extern "C" {
#endif

#define __output_buf_size__			4096

typedef struct in_addr		sin_ip_t;

typedef struct _tag_outbuf_t {
	int		bsize;
	int		dsize;
	char	buf[0];
} outbuf_t, *outbufp_t;

#if defined(__cplusplus)
}
#endif

int ipconv_aton( outbufp_t ob, const char *ip, int len ) {
	int ret = 0;
	sin_ip_t ipnst;

	ret = inet_aton( ip, &ipnst );
	if ( ret == -1 ) {
		return -1;
	}

	ret = snprintf( ob->buf+ob->dsize, ob->bsize-ob->dsize, "Network-Byte-Order:\t%u (signed:%d)\n", ipnst.s_addr, ipnst.s_addr );
	if ( ret < 0 ) {
		return -2;
	}
	ob->dsize += ret;

	ret = snprintf( ob->buf+ob->dsize, ob->bsize-ob->dsize, "Host-Byte-Order:\t%u (signed:%d)\n", ntohl(ipnst.s_addr), ntohl(ipnst.s_addr) );
	if ( ret < 0 ) {
		return -3;
	}
	ob->dsize += ret;

	return ret;
}

int ipconv_ntoa( outbufp_t ob, const char *ipn, int len ) {
	int ret = 0;
	sin_ip_t ipnst;
	const char *ips = NULL;

	ipnst.s_addr = (unsigned int)atoi( ipn );

	ips = inet_ntoa( ipnst );
	if ( ips == NULL ) {
		return -1;
	}
	ret = snprintf( ob->buf+ob->dsize, ob->bsize-ob->dsize, "Network-Byte-Order Based:\t%s\n", ips );
	if ( ret < 0 ) {
		return -2;
	}
	ob->dsize += ret;

	ipnst.s_addr = htonl( ipnst.s_addr );
	ips = inet_ntoa( ipnst );
	if ( ips == NULL ) {
		return -2;
	}
	ret = snprintf( ob->buf+ob->dsize, ob->bsize-ob->dsize, "Host-Byte-Order Based:\t\t%s\n", ips );
	if ( ret < 0 ) {
		return -2;
	}

	return ret;
}

void show_usage(void) {
	printf( "command ip-number/ip-string\n" );
}

int main( int argc, char *argv[] ) {
	int ret = 0;
	int arg_len = 0;
	outbufp_t	output;


	if ( argc < 2 ) {
		printf( "Arguments is absent! Aborted\n" );
		show_usage();
		return 0;
	}

	output = (outbufp_t)malloc( __output_buf_size__ );
	if ( output == NULL ) {
		printf( "Output buffer malloc failed! Aborted\n" );
		return 0;
	}
	memset( output, 0, __output_buf_size__ );
	output->bsize = __output_buf_size__ - sizeof(outbuf_t);
	output->dsize = 0;

	arg_len = strlen( argv[1] );
	if ( strchr(argv[1], '.') == NULL ) {
		/* arg is ip-number */
		ret = ipconv_ntoa( output, argv[1], arg_len );
	}  else {
		/* arg is ip-string */
		ret = ipconv_aton( output, argv[1], arg_len );
	}

	printf( "%s\n", output->buf );

	if ( output ) {
		free( output );
	}


	return 0;
}
