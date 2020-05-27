#ifndef SL_PARSER_H
#define SL_PARSER_H

typedef const char *sl_parser_error_t;

typedef struct _sl_parser_t
{
	sl_lexer_t *lexer;
	sl_token_t *current_token;
	sl_token_t *erroring_token;
} sl_parser_t;

typedef struct _sl_parser_result_t
{
	bool is_error;

	union
	{
		sl_parser_node_t *node;
		sl_parser_error_t error;
	};
} sl_parser_result_t;

sl_parser_result_t sl_parse_primary_expression( sl_parser_t *parser );
sl_parser_result_t sl_parse_unary_expression( sl_parser_t *parser );
sl_parser_result_t sl_parse_multiplication_expression( sl_parser_t *parser );
sl_parser_result_t sl_parse_addition_expression( sl_parser_t *parser );
sl_parser_result_t sl_parse_comparison_expression( sl_parser_t *parser );
sl_parser_result_t sl_parse_equality_expression( sl_parser_t *parser );
sl_parser_result_t sl_parse_expression( sl_parser_t *parser );

sl_parser_result_t sl_parse_function_definition( sl_parser_t *parser );
sl_parser_result_t sl_parse_statement( sl_parser_t *parser );

/* Initializes the parser structure.
   Always returns SL_RES_OK. */
sl_result_t sl_parser_init( sl_parser_t *parser, sl_lexer_t *lexer );

/* Processes the given token stream.
   Allocates a buffer for resulting AST nodes. */
sl_result_t sl_parser_digest( sl_parser_t *parser, sl_token_t *tokens, int32_t token_count,
	sl_parser_node_t **out_ast_nodes, int32_t *ast_node_count, sl_parser_error_t *out_error );

/* Prints the gives parser error.
   Always returns SL_RES_OK. */
sl_result_t sl_parser_error_print( sl_parser_t *parser, sl_parser_error_t *error );

#endif // SL_PARSER_H
