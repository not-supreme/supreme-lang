#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "../../inc/parser/parser.h"

#define PARSER_MAX_LOAD 0.75
#define GROW_CAPACITY( capacity ) ( ( capacity ) < 8 ? 8 : ( capacity ) * 2 )

void parser_parse_expression( parser_t *parser );
void parser_parse_grouping_expression( parser_t *parser );
void parser_parse_binary_expression( parser_t *parser );
void parser_parse_number_literal( parser_t *parser );
void parser_parse_integer_literal( parser_t *parser );
void parser_parse_string_literal( parser_t *parser );

//	https://en.wikipedia.org/wiki/Pratt_parser ^^
parse_rule_t parse_rules[ ] =
{
	{ NULL, parser_parse_binary_expression, TOKEN_OPERATOR_ADD, PRECEDENCE_TERM },
	{ NULL, parser_parse_binary_expression, TOKEN_OPERATOR_SUB, PRECEDENCE_TERM },
	{ NULL, parser_parse_binary_expression, TOKEN_OPERATOR_DIV, PRECEDENCE_FACTOR },
	{ NULL, parser_parse_binary_expression, TOKEN_OPERATOR_MUL, PRECEDENCE_FACTOR },
	{ NULL, parser_parse_binary_expression, TOKEN_OPERATOR_REM, PRECEDENCE_TERM },

	{ parser_parse_grouping_expression, NULL, TOKEN_OPENING_PAREN, PRECEDENCE_NONE },
	{ NULL, NULL, TOKEN_CLOSING_PAREN, PRECEDENCE_NONE },

	{ parser_parse_number_literal, NULL, TOKEN_NUMBER, PRECEDENCE_NONE },
	{ parser_parse_integer_literal, NULL, TOKEN_INTEGER, PRECEDENCE_NONE },
	{ parser_parse_string_literal, NULL, TOKEN_STRING, PRECEDENCE_NONE },
	{ NULL, NULL, TOKEN_EOF, PRECEDENCE_NONE },
};

parse_rule_t *get_parse_rule( e_token_type token_type )
{
	parse_rule_t *parse_rule = NULL;

	for ( int i = 0; parse_rule == NULL && i < sizeof( parse_rules ) / sizeof( parse_rule_t ); i++ )
	{
		if ( parse_rules[ i ].token_type == token_type )
			parse_rule = &parse_rules[ i ];
	}

	return parse_rule;
}

void parser_init( parser_t *parser )
{
	parser->length = 0;
	parser->capacity = -1;
	parser->index = 0;
	parser->tokens = NULL;
}

void parser_free( parser_t *parser )
{
	free( parser->tokens );

	parser_init( parser );
}

void parser_adjust_capacity( parser_t *parser )
{
	if ( ( int64_t )parser->length + 1 > ( ( int64_t )parser->capacity + 1 ) * PARSER_MAX_LOAD )
	{
		int capacity = GROW_CAPACITY( parser->capacity + 1 ) - 1;

		token_t *new_tokens = realloc( parser->tokens, sizeof( token_t ) * capacity );

		//	fixme: dont exit silently, report an error instead!
		if ( new_tokens == NULL )
			exit( 1 );

		parser->capacity = capacity;
		parser->tokens = new_tokens;
	}
}

void parser_process_token( parser_t *parser, token_t *next_token )
{
	parser_adjust_capacity( parser );

	parser->tokens[ parser->length++ ] = *next_token;
}

bool parser_is_eof( parser_t *parser )
{
	return parser->index >= parser->length;
}

token_t *parser_peek_token( parser_t *parser )
{
	if ( parser_is_eof( parser ) )
		return NULL;

	return &parser->tokens[ parser->index ];
}

token_t *parser_consume_token( parser_t *parser )
{
	if ( parser_is_eof( parser ) )
		return NULL;

	return &parser->tokens[ parser->index++ ];
}

bool parser_expect( parser_t *parser, e_token_type token_type )
{
	if ( parser_is_eof( parser ) )
		return false;

	token_t *token = parser_peek_token( parser );

	bool matches = token->token_type == token_type;

	if ( matches )
		parser->index++;

	return matches;
}

void parser_parse_precedence( parser_t *parser, e_precedence precedence )
{
	token_t *token = &parser->tokens[ parser->index++ ];

	parse_rule_t *parse_rule = get_parse_rule( token->token_type );

	//	todo: report error
	if ( parse_rule == NULL )
		return;

	//	todo: report error
	if ( parse_rule->parse_prefix == NULL )
		return;

	parse_rule->parse_prefix( parser );

	while ( !parser_is_eof( parser ) && precedence <= get_parse_rule( parser->tokens[ parser->index++ ].token_type )->precedence )
	{
		get_parse_rule( parser->tokens[ parser->index - 1 ].token_type )->parse_infix( parser );
	}
}

void parser_parse_expression( parser_t *parser )
{
	parser_parse_precedence( parser, PRECEDENCE_ASSIGNMENT );
}

void parser_parse_grouping_expression( parser_t *parser )
{
	printf( "group expression \n" );

	parser_parse_expression( parser );

	//	todo: report error
	if ( !parser_expect( parser, TOKEN_CLOSING_PAREN ) )
		return;
}

void parser_parse_binary_expression( parser_t *parser )
{
	token_t *operator_token = &parser->tokens[ parser->index - 1 ];
	parse_rule_t *parse_rule = get_parse_rule( operator_token->token_type );

	parser_parse_precedence( parser, ( e_precedence )( parse_rule->precedence + 1 ) );

	switch ( operator_token->token_type )
	{
	case TOKEN_OPERATOR_ADD: printf( "add operator! \n" ); break;
	case TOKEN_OPERATOR_SUB: printf( "subtract operator! \n" ); break;
	case TOKEN_OPERATOR_DIV: printf( "divide operator! \n" ); break;
	case TOKEN_OPERATOR_MUL: printf( "multiply operator! \n" ); break;
	case TOKEN_OPERATOR_REM: printf( "remainder operator! \n" ); break;

	default:
		return;
	}
}

void parser_parse_number_literal( parser_t *parser )
{
	token_t *token = &parser->tokens[ parser->index - 1 ];

	printf( "number literal: %f \n", token_get_number( token ) );
}

void parser_parse_integer_literal( parser_t *parser )
{
	token_t *token = &parser->tokens[ parser->index - 1 ];

	printf( "integer literal: %lld \n", token_get_integer( token ) );
}

void parser_parse_string_literal( parser_t *parser )
{
	token_t *token = &parser->tokens[ parser->index - 1 ];

	printf( "string literal: \"%s\" \n", token_get_string( token ) );
}

void parser_run( parser_t *parser )
{
	for ( ; parser->index < parser->length; parser->index++ )
	{
		parser_parse_expression( parser );
	}
}
