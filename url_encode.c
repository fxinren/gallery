#include <stdio.h>
#include <string.h>

char tbl[1024];

void build_url_encode_dict(void) {
	unsigned char c = 0;
	int i = 0;
	int j = 0;

	memset( tbl, 0, 1024 );

	for ( j=0; j<256; ++j ) {
		c = j;
		if ( c>='A' && c<='Z' ) { tbl[i] = c; }
		else if ( c>='a' && c<='z' ) { tbl[i] = c; }
		else if ( c>='0' && c<='9' ) { tbl[i] = c; }
		else {
			switch ( c ) {
			case '-':
			case '_':
			case '.':
			case '~':
				tbl[i] = c;
				break;
			default:
				sprintf(tbl+i, "%%%02X", (unsigned char)c);
				i += 2;
				break;
			}
		}

		++i;
		tbl[i] = ',';
		++i;
	}
}

// Canonicalized Query String
int url_decode( const char *url, char *dest ) {
	return 0;
}

int url_encode( const char *url, char *dest ) {
	return 0;
}

int main( int argc, char *argv[] ) {
	build_url_encode_dict();
	printf( "%s\n", tbl );
}
