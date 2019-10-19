#include "ape_ftw.h"

/*********************************************
 * gcc cpp_conv.c -o cppconv 
*********************************************/

#if defined(__cplusplus)
extern "C" {
#endif

// cmd path
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
