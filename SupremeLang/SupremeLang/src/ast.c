#include "../inc/shared.h"
#include "../inc/result.h"
#include "../inc/token.h"
#include "../inc/ast.h"

sl_result_t sl_parser_node_free( sl_parser_node_t *node )
{
	if ( node->node_type == AST_GROUP_EXPR )
	{
		SL_ASSERT( sl_parser_node_free( node->as.group_expression ) == SL_RES_OK );

		node->as.group_expression = NULL;
	}
	else if ( node->node_type == AST_UNARY_EXPR )
	{
		SL_ASSERT( sl_parser_node_free( node->as.unary_expr.operand ) == SL_RES_OK );

		node->as.unary_expr.operand = NULL;
	}
	else if ( node->node_type == AST_BINARY_EXPR )
	{
		SL_ASSERT( sl_parser_node_free( node->as.binary_expr.left ) == SL_RES_OK );
		SL_ASSERT( sl_parser_node_free( node->as.binary_expr.right ) == SL_RES_OK );

		node->as.binary_expr.left = NULL;
		node->as.binary_expr.right = NULL;
	}

	return SL_RES_OK;
}
