#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define is_digital(c)			(c>='0'&&c<='9')

char line[1024];
int len;

//00:04:15,986 --> 00:04:21,334
#define tm_min_fmt					"00:00:00,000"
#define tm_max_fmt					"59:59:59,999"

int isnumber( const char *s )
{
	int yes = 1;
	if ( *s == 0 ) {
		yes = 0;
	}

	if ( (*s)=='-' || (*s)=='+' ) {
		++s;
	}

	while ( *s ) {
		if ( (*s)<'0' || (*s)>'9' ) {
			yes = 0;
			break;
		}
		++s;
	}

	return yes;
}

// return next position to the time
char* parse_time( char *buf, int *sec, int *msec, char **head )
{
	int i = 0;
	int n = 0;
	char *pos = NULL;
	int sum_sec = 1;
	const char *fmt1 = tm_min_fmt;
	const char *fmt2 = tm_max_fmt;

	*sec = 0;
	*msec = 0;

	while ( buf[i] && !is_digital(buf[i]) ) {
		++i;
	}

	*head = buf + i;
	//printf( "%s\n", &(buf[i]) );
	while ( *fmt1 && buf[i] ) {
		if ( buf[i]<(*fmt1) || buf[i]>(*fmt2) ) {
			break;
		}

		if ( (*fmt1) == ',' ) {
			(*sec) = (*sec)*60 + n;
			//printf( "+%d\n", (*sec) );
			n = 0;
			sum_sec = 0;
		} else if ( sum_sec ) {
			if ( (*fmt1) == ':' ) {
				(*sec) = (*sec)*60 + n;
				n = 0;
			} else {
				//(*sec) = (*sec)*10 + buf[i] - '0';
				n = n*10 + buf[i] - '0';
			}
		} else {
			(*msec) = (*msec)*10 + buf[i] - '0';
			//printf( "+%d\n", (*msec) );
		}

		++i;
		++fmt1;
		++fmt2;
	}

	if ( (*fmt1) ) {
		return NULL;
	}

	return (buf + i);
}

int time_2str( int t, char *buf, int len )
{
	int i = 0;
	char tm[3][3];
	char s[13] = {0};
	char *sp = s;
	int ms = 0;
	int n = 0;

	memset( tm, 0, sizeof(tm) );

	ms = t % 1000;
	t /= 1000;

	while ( i < 3 ) {
		n = t % 60;
		t /= 60;
		snprintf( tm[i], 3, "%02d", n );
		++i;
	}

	strcpy( s, tm[2] );
	for ( i=1; i>=0; --i ) {
		strcat( s, ":" );
		strcat( s, tm[i] );
	}

	strcat( s, "," );
	snprintf( s + 9, 4, "%03d", ms );

	while ( *sp ) {
		*buf = *sp;
		++buf;
		++sp;
	}

	//buf[12] = 0;

	return 0;
}

int move( char *buf, int delta )
{
	int s1 = 0;
	int ms1 = 0;
	int s2 = 0;
	int ms2 = 0;
	char *rpl1 = NULL;
	char *rpl2 = NULL;
	char *pos = buf;

	//int delta_sec = 0;
	//int delta_msec = 0;

	char t1[13] = {0};
	char t2[13] = {0};

	
	pos = parse_time( pos, &s1, &ms1, &rpl1 );
	if ( pos == NULL ) {
		return -1;
	}

	pos = parse_time( pos , &s2, &ms2, &rpl2 );
	if ( pos == NULL ) {
		return -1;
	}
	s1 = s1 * 1000 + ms1;
	s2 = s2 * 1000 + ms2;
	//printf( "%d\n", s1);

	s1 += delta;
	s2 += delta;

	if ( s1 < 0 ) {
		printf( "delta time is too large:%s,%d --> %d, %d\n", line, s1, s2, delta);
		return -1;
	}

	time_2str( s1, rpl1, 13 );
	time_2str( s2, rpl2, 13 );

	return 0;
}

int main( int argc, char *argv[] )
{
	FILE *f = NULL;
	FILE *fnew = NULL;
	const char *fname = NULL;
	char fname_new[256] = {0};
	const char *direction = NULL;
	int delta = 0;
	int row = 0;

	int save = 1;

	if ( argc < 3 ) {
		printf( "usage %s file-name.srt [+/-]milli-seconds\n", argv[0] );
		return 0;
	}

	fname = argv[1];
	if ( fname == NULL ) {
		return 0;
	}

	direction = argv[2];
	if ( !isnumber(direction) ) {
		printf( "Invalide movement:%s\n", direction );
		return 0;
	}
	delta = atoi( direction );

	f = fopen( fname, "r" );
	if ( f == NULL ) {
		printf( "Open srt failed:%s\n", fname );
		return 0;
	}

	strcpy( fname_new, fname );
	strcat( fname_new, ".swap" );
	fnew = fopen( fname_new, "w+" );
	if ( fnew == NULL ) {
		printf( "Open new srt failed:%s\n", fname_new );
		return 0;
	}

	//printf( "%d\n", delta );
	while ( fgets(line, 1024, f) ) {
		if ( strstr(line, "-->") ) {
			//printf( "%s\n", line );
			if ( move(line, delta) == -1 ) {
				save = 0;
				printf( "parse time failed[%d]:%s\n", row, line);
				break;
			}
			//printf( "%s\n", line );
		}

		//printf( "---%s\n", line );
		if ( fputs(line, fnew) == EOF ) {
			save = 0;
			printf( "Writing failed[%d]:%s\n", row, line);
			break;
		}
		++row;
		//break;
	}

	fclose( f );
	fclose( fnew );

	if ( save ) {
		// remove old
		save = unlink(fname);
		// rename fnew
		if ( save ) {
			perror( "unlink failed" );
		} else  {
			save = rename( fname_new, fname );
			if ( save ) {
				perror( "rename failed" );
			}
		}
	}

	return 0;
}
