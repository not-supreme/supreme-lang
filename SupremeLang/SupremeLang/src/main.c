#include <stdint.h>
#include <stdio.h>

#include "../inc/lexer/lexer.h"
#include "../inc/lexer/token.h"
#include "../inc/parser/parser.h"

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
		parser_t parser;

		lexer_init( &lexer, line_buffer );
		parser_init( &parser );

		for ( ;; )
		{
			token_t token = lexer_scan_token( &lexer );

			if ( lexer.current_error.has_error )
			{
				printf( "error at %d:%d: %s \n", lexer.current_error.line_number,
					lexer.current_error.column_number, lexer.current_error.description );

				break;
			}

			if ( token.token_type == TOKEN_SKIP )
				continue;

			parser_process_token( &parser, &token );

			if ( token.token_type == TOKEN_EOF )
				break;
		}

		parser_run( &parser );

		parser_free( &parser );
	}

	return 0;
}
