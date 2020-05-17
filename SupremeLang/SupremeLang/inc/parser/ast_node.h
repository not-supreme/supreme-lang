#ifndef SUPREMELANG_AST_NODE_H
#define SUPREMELANG_AST_NODE_H

#include "../lexer/token.h"
#include "ast_node_type.h"

typedef struct _ast_node_t
{
	e_ast_node_type node_type;
	
	union
	{
		token_t *constant_value;

		struct _ast_node_t *group_expression;
		struct _unary_expr_t
		{
			struct _ast_node_t *operand;
			token_t *op_token;
		} unary_expr;
		struct _binary_expr_t
		{
			struct _ast_node_t *left;
			struct _ast_node_t *right;
			token_t *op_token;
		} binary_expr;
	} as;
} ast_node_t;

ast_node_t *ast_node_new( e_ast_node_type node_type );

void ast_node_print( ast_node_t *node );

#endif // SUPREMELANG_AST_NODE_H
