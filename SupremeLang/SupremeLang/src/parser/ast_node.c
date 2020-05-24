#pragma warning( disable: 26451 )

#include <stdio.h>
#include <stdlib.h>

#include "../../inc/parser/ast_node.h"

ast_node_t *ast_node_new( e_ast_node_type node_type )
{
	ast_node_t *node = malloc( sizeof( ast_node_t ) );

	if ( !node )
		return NULL;

	node->node_type = node_type;

	return node;
}

void ast_node_print_internal( ast_node_t *node, bool print_nl )
{
	if ( node->node_type == AST_NODE_CONSTANT )
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
	else if ( node->node_type == AST_NODE_VARIABLE )
	{
		token_t *identifier_token = node->as.variable;

		char span_end_ch = identifier_token->span_end[ 0 ];

		identifier_token->span_end[ 0 ] = '\x00';

		printf( print_nl ? "%s \n" : "%s", identifier_token->span_start );

		identifier_token->span_end[ 0 ] = span_end_ch;
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

		ast_node_print_internal( node->as.binary_expr.right, print_nl );
	}
	else if ( node->node_type == AST_NODE_CALL_EXPRESSION )
	{
		ast_node_print_internal( node->as.call_expr.call_target, false );

		printf( print_nl ? "( ) \n" : "( )" );
	}
	else if ( node->node_type == AST_NODE_EXPRESSION_STATEMENT )
	{
		ast_node_print_internal( node->as.expression_stmt, print_nl );
	}
	else if ( node->node_type == AST_NODE_SWITCH_STATEMENT )
	{
		printf( "switch ( " );

		ast_node_print_internal( node->as.switch_stmt.expression, false );

		printf( print_nl ? " ) { } \n" : " ) { }" );
	}
	else if ( node->node_type == AST_NODE_FOR_STATEMENT )
	{
	}
	else if ( node->node_type == AST_NODE_WHILE_STATEMENT )
	{
		printf( "while ( " );

		ast_node_print_internal( node->as.while_stmt.expression, false );

		printf( print_nl ? " ) { } \n" : " ) { }" );
	}
	else if ( node->node_type == AST_NODE_IF_STATEMENT )
	{
	}
	else if ( node->node_type == AST_NODE_VARIABLE_DECLARATION )
	{
		token_t *identifier_token = node->as.variable_def.identifier;
		token_t *type_identifier_token = node->as.variable_def.type;

		char span_end_ch_ident = identifier_token->span_end[ 0 ];
		char span_end_ch_type = type_identifier_token->span_end[ 0 ];

		identifier_token->span_end[ 0 ] = '\x00';
		type_identifier_token->span_end[ 0 ] = '\x00';

		if ( node->as.variable_def.initial_value == NULL )
		{
			printf( print_nl ? "var %s: %s \n" : "var %s: %s", identifier_token->span_start, type_identifier_token->span_start );
		}
		else
		{
			printf( "var %s: %s = ", identifier_token->span_start, type_identifier_token->span_start );

			ast_node_print_internal( node->as.variable_def.initial_value, print_nl ); 
		}

		identifier_token->span_end[ 0 ] = span_end_ch_ident;
		type_identifier_token->span_end[ 0 ] = span_end_ch_type;
	}
	else if ( node->node_type == AST_NODE_FUNCTION_DECLARATION )
	{
		token_t *identifier_token = node->as.function_def.identifier;

		char span_end_ch = identifier_token->span_end[ 0 ];

		identifier_token->span_end[ 0 ] = '\x00';

		printf( print_nl ? "fn %s( /* %d arguments */ ) { } \n" : "fn %s( /* %d arguments */ ) { }",
			identifier_token->span_start, node->as.function_def.argument_arity );

		identifier_token->span_end[ 0 ] = span_end_ch;
	}
}

void ast_node_print( ast_node_t *node )
{
	ast_node_print_internal( node, true );
}
