#include "../inc/shared.h"
#include "../inc/result.h"
#include "../inc/token.h"
#include "../inc/lexer.h"
#include "../inc/ast.h"
#include "../inc/parser.h"

sl_result_t reallocate_ast_nodes_buffer( sl_parser_node_t **out_ast_nodes, int32_t *capacity )
{
	// Adjust the capacity of the AST nodes buffer.
	*capacity = ( ( *capacity ) < 16 ) ? 16 : ( *capacity ) * 2;

	// Check if the AST nodes buffer was originally allocated.
	// If not, allocate it with the initial capacity.
	if ( *out_ast_nodes == NULL )
	{
		*out_ast_nodes = malloc( sizeof( sl_parser_node_t ) * ( *capacity ) );

		SL_ASSERT( *out_ast_nodes != NULL );
	}
	// Else, reallocate the existing memory block.
	else
	{
		sl_parser_node_t *new_buffer = realloc( *out_ast_nodes, sizeof( sl_parser_node_t ) * ( *capacity ) );

		SL_ASSERT( new_buffer != NULL );

		*out_ast_nodes = new_buffer;
	}

	return SL_RES_OK;
}

sl_result_t sl_parser_init( sl_parser_t *parser, sl_lexer_t *lexer )
{
	parser->lexer = lexer;

	return SL_RES_OK;
}

sl_parser_result_t sl_parse_primary_expression( sl_parser_t *parser )
{
	sl_parser_result_t result;

	result.is_error = true;
	result.node = malloc( sizeof( sl_parser_node_t ) );

	if ( parser->current_token->token_type == TOKEN_IDENTIFIER )
	{
		result.is_error = false;
		result.node->node_type = AST_VARIABLE;
		result.node->as.variable = parser->current_token++;
	}
	else if ( ( parser->current_token->token_type >= TOKEN_NUMBER && parser->current_token->token_type <= TOKEN_STRING )
		|| parser->current_token->token_type == TOKEN_KEYWORD_TRUE || parser->current_token->token_type == TOKEN_KEYWORD_FALSE )
	{
		result.is_error = false;
		result.node->node_type = AST_CONSTANT;
		result.node->as.constant = parser->current_token++;
	}
	else if ( parser->current_token->token_type == TOKEN_LEFT_PAREN )
	{
		parser->current_token++;

		if ( parser->current_token->token_type == TOKEN_EOF )
		{
			if ( result.node != NULL )
			{
				sl_parser_node_free( result.node );
				free( result.node );
			}

			result.error = "expected identifier, number, integer or string literal";

			parser->erroring_token = parser->current_token;

			return result;
		}

		sl_parser_result_t nested_expr = sl_parse_expression( parser );

		if ( nested_expr.is_error )
		{
			if ( result.node != NULL )
			{
				sl_parser_node_free( result.node );
				free( result.node );
			}

			return nested_expr;
		}

		if ( ( parser->current_token++ )->token_type != TOKEN_RIGHT_PAREN )
		{
			if ( result.node != NULL )
			{
				sl_parser_node_free( result.node );
				free( result.node );
			}

			if ( nested_expr.node != NULL )
			{
				sl_parser_node_free( nested_expr.node );
				free( nested_expr.node );
			}

			result.error = "expected closing parenthesis";

			parser->erroring_token = ( parser->current_token - 1 );

			return result;
		}

		result.is_error = false;
		result.node->node_type = AST_GROUP_EXPR;
		result.node->as.group_expression = nested_expr.node;
	}
	else
	{
		if ( result.node != NULL )
		{
			sl_parser_node_free( result.node );
			free( result.node );
		}

		result.error = "unexpected token";

		parser->erroring_token = parser->current_token;
	}

	return result;
}

sl_parser_result_t sl_parse_unary_expression( sl_parser_t *parser )
{
	if ( parser->current_token->token_type == TOKEN_BANG || parser->current_token->token_type == TOKEN_MINUS )
	{
		sl_parser_result_t result;

		result.is_error = false;
		result.node = malloc( sizeof( sl_parser_node_t ) );

		result.node->node_type = AST_UNARY_EXPR;
		result.node->as.unary_expr.op_token = parser->current_token++;

		sl_parser_result_t nested_expr = sl_parse_unary_expression( parser );

		if ( nested_expr.is_error )
		{
			if ( result.node != NULL )
			{
				sl_parser_node_free( result.node );
				free( result.node );
			}

			return nested_expr;
		}

		result.node->as.unary_expr.operand = nested_expr.node;

		return result;
	}

	return sl_parse_primary_expression( parser );
}

sl_parser_result_t sl_parse_multiplication_expression( sl_parser_t *parser )
{
	sl_parser_result_t nested_lhs_expr = sl_parse_unary_expression( parser );

	if ( nested_lhs_expr.is_error )
		return nested_lhs_expr;

	while ( parser->current_token->token_type == TOKEN_STAR || parser->current_token->token_type == TOKEN_SLASH )
	{
		sl_parser_result_t result;

		result.is_error = false;
		result.node = malloc( sizeof( sl_parser_node_t ) );

		result.node->node_type = AST_BINARY_EXPR;
		result.node->as.binary_expr.op_token = parser->current_token++;

		sl_parser_result_t nested_rhs_expr = sl_parse_unary_expression( parser );

		if ( nested_rhs_expr.is_error )
		{
			if ( result.node != NULL )
			{
				sl_parser_node_free( result.node );
				free( result.node );
			}

			if ( nested_lhs_expr.node != NULL )
			{
				sl_parser_node_free( nested_lhs_expr.node );
				free( nested_lhs_expr.node );
			}

			return nested_rhs_expr;
		}

		result.node->as.binary_expr.left = nested_lhs_expr.node;
		result.node->as.binary_expr.right = nested_rhs_expr.node;

		nested_lhs_expr = result;
	}

	return nested_lhs_expr;
}

sl_parser_result_t sl_parse_addition_expression( sl_parser_t *parser )
{
	sl_parser_result_t nested_lhs_expr = sl_parse_multiplication_expression( parser );

	if ( nested_lhs_expr.is_error )
		return nested_lhs_expr;

	while ( parser->current_token->token_type == TOKEN_PLUS || parser->current_token->token_type == TOKEN_MINUS )
	{
		sl_parser_result_t result;

		result.is_error = false;
		result.node = malloc( sizeof( sl_parser_node_t ) );

		result.node->node_type = AST_BINARY_EXPR;
		result.node->as.binary_expr.op_token = parser->current_token++;

		sl_parser_result_t nested_rhs_expr = sl_parse_multiplication_expression( parser );

		if ( nested_rhs_expr.is_error )
		{
			if ( result.node != NULL )
			{
				sl_parser_node_free( result.node );
				free( result.node );
			}

			if ( nested_lhs_expr.node != NULL )
			{
				sl_parser_node_free( nested_lhs_expr.node );
				free( nested_lhs_expr.node );
			}

			return nested_rhs_expr;
		}

		result.node->as.binary_expr.left = nested_lhs_expr.node;
		result.node->as.binary_expr.right = nested_rhs_expr.node;

		nested_lhs_expr = result;
	}

	return nested_lhs_expr;
}

sl_parser_result_t sl_parse_comparison_expression( sl_parser_t *parser )
{
	sl_parser_result_t nested_lhs_expr = sl_parse_addition_expression( parser );

	if ( nested_lhs_expr.is_error )
		return nested_lhs_expr;

	while ( parser->current_token->token_type == TOKEN_GREATER || parser->current_token->token_type == TOKEN_GREATER_EQUAL
		|| parser->current_token->token_type == TOKEN_LESS || parser->current_token->token_type == TOKEN_LESS_EQUAL )
	{
		sl_parser_result_t result;

		result.is_error = false;
		result.node = malloc( sizeof( sl_parser_node_t ) );

		result.node->node_type = AST_BINARY_EXPR;
		result.node->as.binary_expr.op_token = parser->current_token++;

		sl_parser_result_t nested_rhs_expr = sl_parse_addition_expression( parser );

		if ( nested_rhs_expr.is_error )
		{
			if ( result.node != NULL )
			{
				sl_parser_node_free( result.node );
				free( result.node );
			}

			if ( nested_lhs_expr.node != NULL )
			{
				sl_parser_node_free( nested_lhs_expr.node );
				free( nested_lhs_expr.node );
			}

			return nested_rhs_expr;
		}

		result.node->as.binary_expr.left = nested_lhs_expr.node;
		result.node->as.binary_expr.right = nested_rhs_expr.node;

		nested_lhs_expr = result;
	}

	return nested_lhs_expr;
}

sl_parser_result_t sl_parse_equality_expression( sl_parser_t *parser )
{
	sl_parser_result_t nested_lhs_expr = sl_parse_comparison_expression( parser );

	if ( nested_lhs_expr.is_error )
		return nested_lhs_expr;

	while ( parser->current_token->token_type == TOKEN_BANG_EQUAL || parser->current_token->token_type == TOKEN_EQUAL_EQUAL )
	{
		sl_parser_result_t result;

		result.is_error = false;
		result.node = malloc( sizeof( sl_parser_node_t ) );

		result.node->node_type = AST_BINARY_EXPR;
		result.node->as.binary_expr.op_token = parser->current_token++;

		sl_parser_result_t nested_rhs_expr = sl_parse_comparison_expression( parser );

		if ( nested_rhs_expr.is_error )
		{
			if ( result.node != NULL )
			{
				sl_parser_node_free( result.node );
				free( result.node );
			}

			if ( nested_lhs_expr.node != NULL )
			{
				sl_parser_node_free( nested_lhs_expr.node );
				free( nested_lhs_expr.node );
			}

			return nested_rhs_expr;
		}

		result.node->as.binary_expr.left = nested_lhs_expr.node;
		result.node->as.binary_expr.right = nested_rhs_expr.node;

		nested_lhs_expr = result;
	}

	return nested_lhs_expr;
}

sl_parser_result_t sl_parse_expression( sl_parser_t *parser )
{
	return sl_parse_equality_expression( parser );
}

sl_parser_result_t sl_parse_function_definition( sl_parser_t *parser )
{
	sl_parser_result_t result;

	result.is_error = true;
	result.error = "not implemented yet";

	return result;
}

sl_parser_result_t sl_parse_statement( sl_parser_t *parser )
{
	if ( parser->current_token->token_type == TOKEN_KEYWORD_FN )
	{
		return sl_parse_function_definition( parser );
	}
	else
	{
		sl_parser_result_t expression = sl_parse_expression( parser );

		if ( expression.is_error )
			return expression;

		if ( ( parser->current_token++ )->token_type != TOKEN_SEMICOLON )
		{
			if ( expression.node != NULL )
			{
				sl_parser_node_free( expression.node );
				free( expression.node );
			}

			expression.is_error = true;
			expression.error = "expected semicolon";

			parser->erroring_token = ( parser->current_token - 1 );

			return expression;
		}

		sl_parser_result_t result;

		result.is_error = false;
		result.node = malloc( sizeof( sl_parser_node_t ) );

		result.node->node_type = AST_EXPR_STATEMENT;
		result.node->as.expr_statement = expression.node;

		return result;
	}
}

sl_result_t sl_parser_digest( sl_parser_t *parser, sl_token_t *tokens, int32_t token_count,
	sl_parser_node_t **out_ast_nodes, int32_t *ast_node_count, sl_parser_error_t *out_error )
{
	// Variables used to keep track of how many nodes there
	// are and how many nodes the buffer can hold.
	int32_t node_buf_capacity = 1;
	int32_t node_amount = 0;

	// AST nodes buffer. Initialized to null.
	sl_parser_node_t *ast_nodes_buf = NULL;

	// Allocate the token buffer and make sure that
	// the allocation didn't fail.
	SL_ASSERT( reallocate_ast_nodes_buffer( &ast_nodes_buf, &node_buf_capacity ) == SL_RES_OK );
	SL_ASSERT( ast_nodes_buf != NULL );

	*out_ast_nodes = ast_nodes_buf;
	*ast_node_count = node_amount;

	// Set the current token to the first token available.
	parser->current_token = &tokens[ 0 ];
	parser->erroring_token = parser->current_token;

	// Break out of the loop if the current token is EOF.
	while ( parser->current_token->token_type != TOKEN_EOF )
	{
		// Check if we have enough space for nodes.
		// If not, allocate some more memory!
		if ( node_amount >= ( node_buf_capacity * 0.75 ) )
		{
			SL_ASSERT( reallocate_ast_nodes_buffer( &ast_nodes_buf, &node_buf_capacity ) == SL_RES_OK );
			SL_ASSERT( ast_nodes_buf != NULL );

			*out_ast_nodes = ast_nodes_buf;
			*ast_node_count = node_amount;
		}

		// Run the statement parser.
		sl_parser_result_t parse_result = sl_parse_statement( parser );

		// Return back to the caller if the parser function return an error.
		if ( parse_result.is_error )
		{
			*out_error = parse_result.error;

			return SL_RES_PARSE_ERROR;
		}

		// Copy the resulting AST node over and free the previous one.
		memcpy( &ast_nodes_buf[ node_amount++ ], parse_result.node, sizeof( sl_parser_node_t ) );
		free( parse_result.node );
	}

	*out_ast_nodes = ast_nodes_buf;
	*ast_node_count = node_amount;

	return SL_RES_OK;
}

sl_result_t sl_parser_error_print( sl_parser_t *parser, sl_parser_error_t *error )
{
	sl_token_t *token = parser->erroring_token;

	int32_t line_number = token->line_number + 1;
	int32_t pad_size = 0;

	while ( line_number > 0 )
	{
		line_number /= 10;
		pad_size += 1;
	}

	printf( "error: %s \n", *error );
	printf( "at repl:%d:%d \n", token->line_number + 1, token->column_number + 1 );

	printf( " %*s | \n", pad_size, "" );
	printf( " %-*d | %s \n", pad_size, token->line_number + 1, parser->lexer->source_code );
	printf( " %*s | %*s^ \n", pad_size, "", token->column_number, "" );

	return SL_RES_OK;
}
