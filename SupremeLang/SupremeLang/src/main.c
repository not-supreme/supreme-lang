#include <stdint.h>
#include <stdio.h>

#include "../inc/lexer.h"
#include "../inc/token.h"
#include "../inc/backend/backend_shared.h"
#include "../inc/backend/backend_amd64.h"

int main( int argc, char **argv )
{
	printf( "Supreme-Lang Compiler\n" );

	amd64_assemble32( NULL, NULL, NULL );

	char line_buffer[ 1024 ];

	for ( ;; )
	{
		printf( "> " );

		if ( fgets( line_buffer, sizeof( line_buffer ), stdin ) == NULL )
			continue;

		lexer_t lexer;

		lexer_init( &lexer, line_buffer );

		for ( ;; )
		{
			token_t token = lexer_scan_token( &lexer );

			if ( token.token_type == TOKEN_SKIP )
				continue;

			token_print( &token );

			if ( token.token_type == TOKEN_EOF )
				break;
		}
	}

	return 0;
}
