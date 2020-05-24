#ifndef SUPREMELANG_AST_NODE_H
#define SUPREMELANG_AST_NODE_H

#include "../lexer/token.h"
#include "ast_block.h"
#include "ast_node_type.h"

typedef struct _ast_node_t
{
	e_ast_node_type node_type;

	union
	{
		token_t *constant_value;
		token_t *variable;

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
		struct _call_expr_t
		{
			int argument_count;
			int arguments_capacity;

			struct _ast_node_t *call_target;
			struct _ast_node_t *arguments;
		} call_expr;

		struct _ast_node_t *expression_stmt;
		struct _switch_stmt_t
		{
			struct _ast_node_t *expression;
		} switch_stmt; //	todo: finish
		struct _for_stmt_t
		{
			ast_block_t loop_body;
		} for_stmt; //	todo: finish
		struct _while_stmt_t
		{
			struct _ast_node_t *expression;

			ast_block_t loop_body;
		} while_stmt;
		struct _if_stmt_t
		{
			struct _ast_node_t *expression;
			
			ast_block_t true_branch_body;
			ast_block_t false_branch_body;

			bool has_else;
		} if_stmt;
		struct _variable_def_stmt_t
		{
			token_t *identifier;
			token_t *type;

			struct _ast_node_t *initial_value;
		} variable_def;
		struct _funtion_def_stmt_t
		{
			token_t *identifier;
			token_t *return_type;

			int argument_arity;
		} function_def;
	} as;
} ast_node_t;

ast_node_t *ast_node_new( e_ast_node_type node_type );

void ast_node_print( ast_node_t *node );

#endif // SUPREMELANG_AST_NODE_H
