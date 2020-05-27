#ifndef SL_AST_H
#define SL_AST_H

typedef enum _sl_e_parser_node_type_t
{
	AST_CONSTANT,
	AST_VARIABLE,
	AST_GROUP_EXPR,
	AST_UNARY_EXPR,
	AST_BINARY_EXPR,
	AST_EXPR_FUNCTION_DEF,
	AST_EXPR_STATEMENT
} sl_e_parser_node_type_t;

typedef struct _sl_parser_node_t
{
	sl_e_parser_node_type_t node_type;

	union
	{
		sl_token_t *constant;
		sl_token_t *variable;

		struct _sl_parser_node_t *group_expression;
		struct _unary_expr_t
		{
			struct _sl_parser_node_t *operand;
			sl_token_t *op_token;
		} unary_expr;
		struct _binary_expr_t
		{
			struct _sl_parser_node_t *left;
			struct _sl_parser_node_t *right;
			sl_token_t *op_token;
		} binary_expr;
		struct _sl_parser_node_t *expr_statement;
	} as;
} sl_parser_node_t;

/* Initializes the AST node structure.
   Always returns SL_RES_OK. */
sl_result_t sl_parser_node_free( sl_parser_node_t *node );

#endif // SL_AST_H
