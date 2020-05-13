#include <stdint.h>
#include <stdio.h>

#include "../inc/lexer.h"
#include "../inc/token.h"
#include "../inc/backend/backend_shared.h"
#include "../inc/backend/backend_amd64.h"

int main( int argc, char **argv )
{
	printf( "Supreme-Lang Compiler\n" );

	{
		uint8_t *encoded;
		size_t size;
		e_backend_error backend_error = amd64_assemble64(
			"xor rax, rax; ret",
			&encoded,
			&size );

		if ( backend_error != BACKEND_SUCCESS )
		{
			printf( "Backend failed!\n" );

			return 1;
		} else
		{
			for ( size_t i = 0; i < size; ++i )
			{
				printf( "%02x ", encoded[ i ] );
			}
			printf( "\n" );
		}
	}

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

			printf( "Token: %s at %d:%d \n", token_type_to_string( token.token_type ), token.line_number, token.column_number );

			if ( token.token_type == TOKEN_EOF )
				break;
		}
	}

	return 0;
}
