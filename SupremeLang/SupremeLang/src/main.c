#include "../inc/shared.h"
#include "../inc/result.h"
#include "../inc/token.h"
#include "../inc/lexer.h"
#include "../inc/ast.h"
#include "../inc/codegen.h"
#include "../inc/parser.h"

int main( )
{
	sl_lexer_t lexer;
	sl_parser_t parser;

	SL_ASSERT( sl_lexer_init( &lexer ) == SL_RES_OK );
	SL_ASSERT( sl_parser_init( &parser, &lexer ) == SL_RES_OK );

	for ( ;; )
	{
		char line_buffer[ 1024 ];

		SL_ASSERT( fwrite( "> ", 1, 2, stdout ) == 2 );
		SL_ASSERT( fgets( line_buffer, sizeof( line_buffer ), stdin ) != NULL );

		if ( line_buffer[ 0 ] == '\n' )
			break;

		// Null terminate the line buffer.
		// That is required by `sl_lexer_digest` method.
		line_buffer[ strlen( line_buffer ) - 1 ] = '\x00';

		int32_t token_count = 0;

		sl_token_t *tokens = NULL;
		sl_lexer_error_t lex_error = NULL;

		if ( sl_lexer_digest( &lexer, line_buffer, &tokens, &token_count, &lex_error ) == SL_RES_OK )
		{
			SL_ASSERT( tokens != NULL );

			int32_t ast_node_count = 0;

			sl_parser_node_t *ast_nodes = NULL;
			sl_parser_error_t parse_error = NULL;

			// Parse the tokens.
			if ( sl_parser_digest( &parser, tokens, token_count, &ast_nodes, &ast_node_count, &parse_error ) == SL_RES_OK )
			{
				sl_codegen_t codegen;
				sl_codegen_error_t codegen_error;
				
				SL_ASSERT( sl_codegen_init( &codegen, "repl" ) == SL_RES_OK );
				
				if ( sl_codegen_compile( &codegen, ast_nodes, ast_node_count, &codegen_error ) != SL_RES_OK )
					SL_DEBUG_LOG( "codegen error: %s", codegen_error );

				SL_ASSERT( sl_codegen_free( &codegen ) == SL_RES_OK );
			}
			else
			{
				SL_ASSERT( sl_parser_error_print( &parser, &parse_error ) == SL_RES_OK );
			}

			// Free the AST nodes.
			for ( int32_t i = 0; ast_nodes != NULL && i < ast_node_count; i++ )
			{
				SL_ASSERT( sl_parser_node_free( &ast_nodes[ i ] ) == SL_RES_OK );

				free( &ast_nodes[ i ] );
			}
		}
		else
		{
			SL_ASSERT( sl_lexer_error_print( &lexer, &lex_error ) == SL_RES_OK );

			lexer.line_number += 1;
			lexer.column_number = 0;
		}

		for ( int32_t i = 0; tokens != NULL && i < token_count; i++ )
			SL_ASSERT( sl_token_free( &tokens[ i ] ) == SL_RES_OK );

		// Free the token buffer.
		if ( tokens != NULL )
			free( tokens );
	}
}
