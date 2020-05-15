#ifndef SUPREMELANG_PARSER_H
#define SUPREMELANG_PARSER_H

#include "../lexer/token.h"
#include "precedence.h"

typedef struct _parser_t
{
	int length;
	int capacity;
	int index;

	token_t *tokens;
} parser_t;

typedef struct _parse_rule_t
{
	void( *parse_prefix )( parser_t * );
	void( *parse_infix )( parser_t * );

	e_token_type token_type;
	e_precedence precedence;
} parse_rule_t;

void parser_init( parser_t *parser );
void parser_free( parser_t *parser );

void parser_process_token( parser_t *parser, token_t *next_token );
void parser_run( parser_t *parser );

#endif // SUPREMELANG_PARSER_H
