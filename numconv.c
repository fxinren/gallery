#include <stdio.h>
#include <stdlib.h>
//#include <limits.h>
#include <string.h>

#define __big_number_bits__			128
#define __big_number_int__			__big_number_bits__/32	

#define __output_buf_size__			4096

typedef struct _tag_big_number_t {
	int n[__big_number_int__]; // 16 bytes, 128 bits
} bignum_t;

typedef struct _tag_outbuf_t {
	int		bsize;
	int		dsize;
	char	buf[0];
} outbuf_t, *outbufp_t;

typedef enum _tag_base_e {
	_base_none_ = 0,
	_base_bin_ = 2,
	_base_oct_ = 8,
	_base_dec_ = 10,
	_base_hex_ = 16
} base_e;

static int str2num( const char *s, bignum_t *num, base_e base )
{
	const char *cp = s;
	int *n = num->n;
	int digit = 0;

	memset( num, 0, sizeof(bignum_t) );

	if ( cp && (*cp)=='-' ) {
		++cp;
	}

	// INT_MAX 2147483647
	// INT_MIN (-INT_MAX - 1)
	while ( cp ) {
		if ( *cp>='0' && *cp<='9' ) {
			digit = *cp - '0';
		} else if ( *cp>='A' && *cp<='F' && base==_base_hex_ ) {
			digit = *cp - 'A' + 10;
		} else if ( *cp>='a' && *cp<='f' && base==_base_hex_) {
			digit = *cp - 'a' + 10;
		} else  {
			break;
		}

		if ( __INT_MAX__-(*n) < digit ) {
			digit -= ( __INT_MAX__ - *n );
			*n = __INT_MAX__;
			++n;
		}

		*n = (*n) * base +  digit;

		++cp;
	}

	return 0;
}

base_e parse_option( int argc, char *argv[], bignum_t *num )
{
	base_e ret = _base_none_;
	//char s[__big_number_bits__] = {0};
	char *s = NULL; 
	char c = 0;
	int i = 0;

	// first argument is an option
	if ( argv[1][0] == '-' ) {
		i = strlen( argv[1] );
		if ( i > 1 ) {
			i = 2; // argv[] index that is number string
			c = argv[1][1];
		} else {
			i = 0;
		}
	} else {
		i = 1; // argv[] index that is number string
		c = 'd';
	}

	switch ( c ) {
	case 'b':
		ret = _base_bin_;
		break;
	case 'o':
		ret = _base_oct_;
		break;
	case 'd':
		ret = _base_dec_;
		break;
	case 'h':
		ret = _base_hex_;
		break;
	default:
		break;
	}

	if ( i > 0 ) {
		s = argv[i];
	}

	//if ( ret != _base_none_ ) {
	if ( ret!=_base_none_ && s!=NULL ) {
		// run the convertion of number
		i = str2num( s, num, ret );
		if ( i < 0 ) {
			ret = _base_none_;
			printf( "oops! Something I do not know goes wrong!\n" );
		}
	}

	return ret;
}

void show_usage( const char *cmd )
{
	printf( "%s -b/-o/-d/-h number\n", cmd );
	printf( "-b/-o/-d/-h to indicate the base of 'number'\n" );
	printf( "\t-b: Binary;\n\t-o: Oct;\n\t-d: Decimal;\n\t-h: Hex\n" );
	printf( "If option absent, -d will be applied, that is, by default 'number' is in decimal\n" );
}

int main( int argc, char *argv[] ) 
{
	int ret = 0;
	int arg_len = 0;
	base_e base = _base_none_;
	bignum_t num;
	outbufp_t output;

	if ( argc < 2 ) {
		printf( "Arguments is absent! Aborted\n" );
		show_usage( argv[0] );
		return 0;
	}

	base = parse_option( argc, argv, &num );
	if ( base == _base_none_ ) {
		printf( "Could NOT determin number base!\n" );
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

	printf( "%s\n", output->buf );

	if ( output ) {
		free( output );
	}
	return 0;
}
