#ifndef SUPREMELANG_PARSER_H
#define SUPREMELANG_PARSER_H

#include "../lexer/token.h"
#include "ast_node.h"
#include "parser_error.h"

typedef struct _parser_t
{
	parser_error_t error;

	int length;
	int capacity;

	token_t *tokens;
	token_t *current_token;
	token_t *previous_token;

	ast_block_t nodes_block;
} parser_t;

void parser_init( parser_t *parser );
void parser_free( parser_t *parser );

bool parser_is_eof( parser_t *parser );
bool parser_matches( parser_t *parser, e_token_type token_type );
bool parser_expect( parser_t *parser, e_token_type token_type );

void parser_consume( parser_t *parser );
void parser_error( parser_t *parser, const char *description );

ast_node_t *parser_parse_expression( parser_t *parser );
ast_node_t *parser_parse_equality_expression( parser_t *parser );
ast_node_t *parser_parse_comparison_expression( parser_t *parser );
ast_node_t *parser_parse_addition_expression( parser_t *parser );
ast_node_t *parser_parse_multiplication_expression( parser_t *parser );
ast_node_t *parser_parse_unary_expression( parser_t *parser );
ast_node_t *parser_parse_call_expression( parser_t *parser );
ast_node_t *parser_parse_primary_expression( parser_t *parser );

ast_node_t *parser_parse_statement( parser_t *parser );
ast_node_t *parser_parse_switch_statement( parser_t *parser );
ast_node_t *parser_parse_for_statement( parser_t *parser );
ast_node_t *parser_parse_while_statement( parser_t *parser );
ast_node_t *parser_parse_if_statement( parser_t *parser );
ast_node_t *parser_parse_variable_declaration( parser_t *parser );
ast_node_t *parser_parse_function_declaration( parser_t *parser );

void parser_process_token( parser_t *parser, token_t *token );
void parser_generate_ast( parser_t *parser );

#endif // SUPREMELANG_PARSER_H
