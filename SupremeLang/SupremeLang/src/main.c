#define _CRT_SECURE_NO_WARNINGS

#include <memory.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "../inc/lexer/lexer.h"
#include "../inc/lexer/token.h"
#include "../inc/parser/parser.h"

void run_script( char *script )
{
	lexer_t lexer;

	parser_t parser;

	lexer_init( &lexer, script );

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

	parser_generate_ast( &parser );

	for ( int i = 0; i < parser.ast_length; i++ )
	{
		ast_node_t *node = &parser.nodes[ i ];

		ast_node_print( node );
	}

	parser_free( &parser );
}

void run_from_file( const char *file_path )
{
	FILE *file = fopen( file_path, "r" );

	//	todo: report error
	if ( file == NULL )
		return;

	fseek( file, 0, SEEK_END );

	int file_size = ( int )ftell( file );

	char *file_buffer = malloc( file_size );

	fseek( file, 0, SEEK_SET );

	//	todo: report error
	if ( file_buffer == NULL )
		return;

	memset( file_buffer, 0, file_size );

	fread( file_buffer, 1, file_size, file );

	fclose( file );

	run_script( file_buffer );
}

void run_from_stdin( )
{
	char line_buffer[ 1024 ];

	for ( ;; )
	{
		printf( "> " );

		if ( fgets( line_buffer, sizeof( line_buffer ), stdin ) == NULL )
			continue;

		run_script( line_buffer );
	}
}

int main( int argc, char **argv )
{
	if ( argc == 1 )
	{
		run_from_stdin( );
	}
	else
	{
		for ( int i = 1; i < argc; i++ )
			run_from_file( argv[ i ] );
	}

	return 0;
}
