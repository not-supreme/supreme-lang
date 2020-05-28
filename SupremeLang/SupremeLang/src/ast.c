#include "../inc/shared.h"
#include "../inc/result.h"
#include "../inc/token.h"
#include "../inc/ast.h"

void sl_parse_node_free_internal( sl_parser_node_t *node, sl_parser_node_t *parent_node )
{
	if ( node->node_type == AST_GROUP_EXPR )
	{
		sl_parse_node_free_internal( node->as.group_expression, node );

		node->as.group_expression = NULL;
	}
	else if ( node->node_type == AST_UNARY_EXPR )
	{
		sl_parse_node_free_internal( node->as.unary_expr.operand, node );

		node->as.unary_expr.operand = NULL;
	}
	else if ( node->node_type == AST_BINARY_EXPR )
	{
		sl_parse_node_free_internal( node->as.binary_expr.left, node );
		sl_parse_node_free_internal( node->as.binary_expr.right, node );

		node->as.binary_expr.left = NULL;
		node->as.binary_expr.right = NULL;
	}
	else if ( node->node_type == AST_EXPR_STATEMENT )
	{
		sl_parse_node_free_internal( node->as.expr_statement, node );

		node->as.expr_statement = NULL;
	}

	if ( parent_node != NULL )
		free( node );
}

sl_result_t sl_parser_node_free( sl_parser_node_t *node )
{
	sl_parse_node_free_internal( node, NULL );

	return SL_RES_OK;
}
