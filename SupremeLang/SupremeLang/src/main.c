#include <stdio.h>

#include "../inc/lexer.h"
#include "../inc/token.h"

int main( int argc, char **argv )
{
	printf( "Supreme-Lang Compiler\n" );

	char line_buffer[ 1024 ];

	for ( ;; )
	{
		printf( "> " );

		if ( fgets( line_buffer, sizeof( line_buffer ), stdin ) == NULL )
			continue;

		lexer_t lexer;
		token_t token;

		token.token_type = TOKEN_UNKNOWN;

		init_lexer( &lexer, line_buffer );

		while ( token.token_type != TOKEN_EOF )
		{
			token = lexer_scan_one( &lexer );

			printf( "Token: %s \n", token_type_to_string( token.token_type ) );
		}
	}

	return 0;
}
