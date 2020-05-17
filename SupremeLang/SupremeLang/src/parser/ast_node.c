#pragma warning( disable: 26451 )

#include <stdio.h>
#include <stdlib.h>

#include "../../inc/parser/ast_node.h"

ast_node_t *ast_node_new( e_ast_node_type node_type )
{
	ast_node_t *node = malloc( sizeof( ast_node_t ) );

	node->node_type = node_type;

	return node;
}

void ast_node_print_internal( ast_node_t *node, bool print_nl )
{
	if (node->node_type == AST_NODE_CONSTANT)
	{
		token_t *const_token = node->as.constant_value;

		if ( const_token->token_type == TOKEN_KEYWORD_TRUE || const_token->token_type == TOKEN_KEYWORD_FALSE )
			printf( print_nl ? "%s \n" : "%s", "false\0true" + ( token_get_boolean( const_token ) * 6 ) );
		else if ( const_token->token_type == TOKEN_INTEGER )
			printf( print_nl ? "%lld \n" : "%lld", token_get_integer( const_token ) );
		else if ( const_token->token_type == TOKEN_NUMBER )
			printf( print_nl ? "%f \n" : "%f", token_get_number( const_token ) );
		else if ( const_token->token_type == TOKEN_STRING )
			printf( print_nl ? "%s \n" : "%s", token_get_string( const_token ) );
	}
	else if ( node->node_type == AST_NODE_GROUP_EXPRESSION )
	{
		printf( "( " );

		ast_node_print_internal( node->as.group_expression, false );

		printf( print_nl ? " ) \n" : " )" );
	}
	else if ( node->node_type == AST_NODE_UNARY_EXPRESSION )
	{
		token_t *op_token = node->as.unary_expr.op_token;

		char span_end_ch = op_token->span_end[ 1 ];

		op_token->span_end[ 1 ] = '\x00';

		printf( "%s", op_token->span_start );

		op_token->span_end[ 1 ] = span_end_ch;

		ast_node_print_internal( node->as.unary_expr.operand, print_nl );
	}
	else if ( node->node_type == AST_NODE_BINARY_EXPRESSION )
	{
		token_t *op_token = node->as.binary_expr.op_token;

		ast_node_print_internal( node->as.binary_expr.left, false );

		char span_end_ch = op_token->span_end[ 1 ];

		op_token->span_end[ 1 ] = '\x00';

		printf( " %s ", op_token->span_start );

		op_token->span_end[ 1 ] = span_end_ch;

		if ( node->as.binary_expr.right->node_type != AST_NODE_CONSTANT )
		{
			ast_node_t *group_node = ast_node_new( AST_NODE_GROUP_EXPRESSION );

			group_node->as.group_expression = node->as.binary_expr.right;

			node->as.binary_expr.right = group_node;
		}

		ast_node_print_internal( node->as.binary_expr.right, print_nl );
	}
}

void ast_node_print( ast_node_t *node )
{
	ast_node_print_internal( node, true );
}
