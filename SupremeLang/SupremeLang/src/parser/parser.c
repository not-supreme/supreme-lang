#include <memory.h>
#include <stdlib.h>

#include "../../inc/parser/parser.h"

#define PARSER_MAX_LOAD 0.75
#define GROW_CAPACITY( capacity ) ( ( capacity ) < 8 ? 8 : ( capacity ) * 2 )

void parser_init( parser_t *parser )
{
	parser->tokens_length = 0;
	parser->tokens_capacity = -1;

	parser->ast_length = 0;
	parser->ast_capacity = 0;

	parser->tokens = NULL;
	parser->current_token = NULL;
	parser->previous_token = NULL;

	parser->nodes = NULL;
}

void parser_free( parser_t *parser )
{
	if ( parser->tokens != NULL )
		free( parser->tokens );

	if ( parser->nodes != NULL )
		free( parser->nodes );

	parser_init( parser );
}

void parser_adjust_tokens_capacity( parser_t *parser )
{
	if ( ( int64_t )parser->tokens_length + 1 > ( ( int64_t )parser->tokens_capacity + 1 ) * PARSER_MAX_LOAD )
	{
		int capacity = GROW_CAPACITY( parser->tokens_capacity + 1 ) - 1;

		token_t *new_tokens = realloc( parser->tokens, sizeof( token_t ) * capacity );

		//	fixme: dont exit silently, report an error instead!
		if ( new_tokens == NULL )
			exit( 1 );

		parser->tokens_capacity = capacity;
		parser->tokens = new_tokens;
	}
}

void parser_adjust_ast_nodes_capacity( parser_t *parser )
{
	if ( ( int64_t )parser->ast_length + 1 > ( ( int64_t )parser->ast_capacity + 1 ) * PARSER_MAX_LOAD )
	{
		int capacity = GROW_CAPACITY( parser->ast_capacity + 1 ) - 1;

		ast_node_t *new_ast_nodes = realloc( parser->nodes, sizeof( ast_node_t ) * capacity );

		//	fixme: dont exit silently, report an error instead!
		if ( new_ast_nodes == NULL )
			exit( 1 );

		parser->ast_capacity = capacity;
		parser->nodes = new_ast_nodes;
	}
}

bool parser_is_eof( parser_t *parser )
{
	return parser->current_token->token_type == TOKEN_EOF;
}

bool parser_matches( parser_t *parser, e_token_type token_type )
{
	return parser->current_token->token_type == token_type;
}

void parser_consume( parser_t *parser )
{
	parser->previous_token = parser->current_token;
	parser->current_token++;
}

void parser_process_token( parser_t *parser, token_t *token )
{
	parser_adjust_tokens_capacity( parser );

	memcpy( &parser->tokens[ parser->tokens_length++ ], token, sizeof( token_t ) );
}

void parser_generate_ast( parser_t *parser )
{
	parser->current_token = &parser->tokens[ 0 ];

	while ( !parser_is_eof( parser ) )
	{
		ast_node_t *expression = parser_parse_expression( parser );

		if (expression != NULL)
		{
			parser_adjust_ast_nodes_capacity( parser );

			memcpy( &parser->nodes[ parser->ast_length++ ], expression, sizeof( ast_node_t ) );

			free( expression );
		}
	}
}
