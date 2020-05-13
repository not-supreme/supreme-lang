#include <stdio.h>

#include "../inc/lexer.h"

int main( int argc, char **argv )
{
	printf( "Supreme-Lang Compiler\n" );

	char line_buffer[ 1024 ];

	for ( ;; )
	{
		printf( "> " );

		if ( fgets( line_buffer, sizeof( line_buffer ), stdin ) == NULL )
			continue;

		Lexer lexer;

		init_lexer( &lexer, line_buffer );

		// read out all tokens
	}

	return 0;
}
