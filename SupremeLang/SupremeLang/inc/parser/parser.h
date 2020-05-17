#ifndef SUPREMELANG_PARSER_H
#define SUPREMELANG_PARSER_H

#include "../lexer/token.h"
#include "ast_node.h"

typedef struct _parser_t
{
	int tokens_length;
	int tokens_capacity;

	int ast_length;
	int ast_capacity;

	token_t *tokens;
	token_t *current_token;
	token_t *previous_token;

	ast_node_t *nodes;
} parser_t;

void parser_init( parser_t *parser );
void parser_free( parser_t *parser );

bool parser_is_eof( parser_t *parser );
bool parser_matches( parser_t *parser, e_token_type token_type );

void parser_consume( parser_t *parser );

ast_node_t *parser_parse_expression( parser_t *parser );
ast_node_t *parser_parse_equality( parser_t *parser );
ast_node_t *parser_parse_comparison( parser_t *parser );
ast_node_t *parser_parse_addition( parser_t *parser );
ast_node_t *parser_parse_multiplication( parser_t *parser );
ast_node_t *parser_parse_unary( parser_t *parser );
ast_node_t *parser_parse_primary( parser_t *parser );

void parser_process_token( parser_t *parser, token_t *token );
void parser_generate_ast( parser_t *parser );

#endif // SUPREMELANG_PARSER_H
