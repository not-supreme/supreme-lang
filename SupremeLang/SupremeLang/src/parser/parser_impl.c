#include <memory.h>
#include <stdlib.h>

#include "../../inc/parser/parser.h"

token_t *parser_ensure( parser_t *parser, e_token_type token_type, const char *description )
{
	if ( !parser_is_eof( parser ) && parser_matches( parser, token_type ) )
	{
		token_t *token = parser->current_token;

		parser_consume( parser );

		return token;
	}

	parser_error( parser, description );

	return NULL;
}

ast_node_t *parser_parse_expression( parser_t *parser )
{
	return parser_parse_equality_expression( parser );
}

ast_node_t *parser_parse_equality_expression( parser_t *parser )
{
	ast_node_t *expr = parser_parse_comparison_expression( parser );

	while ( parser_matches( parser, TOKEN_OPERATOR_EQUAL ) || parser_matches( parser, TOKEN_OPERATOR_NOT_EQUAL ) )
	{
		ast_node_t *node = ast_node_new( AST_NODE_BINARY_EXPRESSION );

		node->as.binary_expr.op_token = parser->current_token;

		parser_consume( parser );

		node->as.binary_expr.left = expr;
		node->as.binary_expr.right = parser_parse_comparison_expression( parser );

		expr = node;
	}

	return expr;
}

ast_node_t *parser_parse_comparison_expression( parser_t *parser )
{
	ast_node_t *expr = parser_parse_addition_expression( parser );

	while ( parser_matches( parser, TOKEN_OPERATOR_GREATER_THAN )
		 || parser_matches( parser, TOKEN_OPERATOR_GREATER_THAN_EQUAL )
		 || parser_matches( parser, TOKEN_OPERATOR_LESS_THAN )
		 || parser_matches( parser, TOKEN_OPERATOR_LESS_THAN_EQUAL ) )
	{
		ast_node_t *node = ast_node_new( AST_NODE_BINARY_EXPRESSION );

		node->as.binary_expr.op_token = parser->current_token;

		parser_consume( parser );

		node->as.binary_expr.left = expr;
		node->as.binary_expr.right = parser_parse_addition_expression( parser );

		expr = node;
	}

	return expr;
}

ast_node_t *parser_parse_addition_expression( parser_t *parser )
{
	ast_node_t *expr = parser_parse_multiplication_expression( parser );

	while ( parser_matches( parser, TOKEN_OPERATOR_ADD )
		 || parser_matches( parser, TOKEN_OPERATOR_SUB )
		 || parser_matches( parser, TOKEN_OPERATOR_REM ) )
	{
		ast_node_t *node = ast_node_new( AST_NODE_BINARY_EXPRESSION );

		node->as.binary_expr.op_token = parser->current_token;

		parser_consume( parser );

		node->as.binary_expr.left = expr;
		node->as.binary_expr.right = parser_parse_multiplication_expression( parser );

		expr = node;
	}

	return expr;
}

ast_node_t *parser_parse_multiplication_expression( parser_t *parser )
{
	ast_node_t *expr = parser_parse_unary_expression( parser );

	while ( parser_matches( parser, TOKEN_OPERATOR_DIV ) || parser_matches( parser, TOKEN_OPERATOR_MUL ) )
	{
		ast_node_t *node = ast_node_new( AST_NODE_BINARY_EXPRESSION );

		node->as.binary_expr.op_token = parser->current_token;

		parser_consume( parser );

		node->as.binary_expr.left = expr;
		node->as.binary_expr.right = parser_parse_unary_expression( parser );

		expr = node;
	}

	return expr;
}

ast_node_t *parser_parse_unary_expression( parser_t *parser )
{
	if ( parser_matches( parser, TOKEN_OPERATOR_NOT ) || parser_matches( parser, TOKEN_OPERATOR_SUB ) )
	{
		ast_node_t *node = ast_node_new( AST_NODE_UNARY_EXPRESSION );

		node->as.unary_expr.op_token = parser->current_token;

		parser_consume( parser );

		node->as.unary_expr.operand = parser_parse_unary_expression( parser );

		return node;
	}

	return parser_parse_call_expression( parser );
}

ast_node_t *parser_parse_call_expression( parser_t *parser )
{
	ast_node_t *expr = parser_parse_primary_expression( parser );

	if ( parser_matches( parser, TOKEN_OPENING_PAREN ) )
	{
		// todo: do the actual argument parsing :)
		while ( !parser_is_eof( parser ) && !parser_matches( parser, TOKEN_CLOSING_PAREN ) )
			parser_consume( parser );

		if ( !parser_ensure( parser, TOKEN_CLOSING_PAREN, "expected closing parenthesis" ) )
			return NULL;

		ast_node_t *node = ast_node_new( AST_NODE_CALL_EXPRESSION );

		node->as.call_expr.call_target = expr;
		node->as.call_expr.argument_count = 0;
		node->as.call_expr.arguments_capacity = 0;
		node->as.call_expr.arguments = NULL;

		return node;
	}

	return expr;
}

ast_node_t *parser_parse_primary_expression( parser_t *parser )
{
	if ( parser->current_token->token_type == TOKEN_IDENTIFIER )
	{
		ast_node_t *node = ast_node_new( AST_NODE_VARIABLE );

		node->as.variable = parser->current_token;

		parser_consume( parser );

		return node;
	}
	else if ( token_type_is_literal( parser->current_token->token_type ) )
	{
		ast_node_t *node = ast_node_new( AST_NODE_CONSTANT );

		node->as.constant_value = parser->current_token;

		parser_consume( parser );

		return node;
	}
	else if ( parser_matches( parser, TOKEN_OPENING_PAREN ) )
	{
		parser_consume( parser );

		ast_node_t *node = ast_node_new( AST_NODE_GROUP_EXPRESSION );
		ast_node_t *expression_node = parser_parse_expression( parser );

		if ( !parser_ensure( parser, TOKEN_CLOSING_PAREN, "expected closing parenthesis" ) )
			return NULL;

		node->as.group_expression = expression_node;

		return node;
	}

	return NULL;
}

ast_node_t *parser_parse_statement( parser_t *parser )
{
	if ( parser_matches( parser, TOKEN_KEYWORD_SWITCH ) )
	{
		return parser_parse_switch_statement( parser );
	}
	else if ( parser_matches( parser, TOKEN_KEYWORD_FOR ) )
	{
		return parser_parse_for_statement( parser );
	}
	else if ( parser_matches( parser, TOKEN_KEYWORD_WHILE ) )
	{
		return parser_parse_while_statement( parser );
	}
	else if ( parser_matches( parser, TOKEN_KEYWORD_IF ) )
	{
		return parser_parse_if_statement( parser );
	}
	else if ( parser_matches( parser, TOKEN_KEYWORD_VAR ) )
	{
		return parser_parse_variable_declaration( parser );
	}
	else if ( parser_matches( parser, TOKEN_KEYWORD_FN ) )
	{
		return parser_parse_function_declaration( parser );
	}
	else
	{
		ast_node_t *expression = parser_parse_expression( parser );

		if ( !parser_ensure( parser, TOKEN_SEMICOLON, "expected semicolon" ) )
			return NULL;

		ast_node_t *statement = ast_node_new( AST_NODE_EXPRESSION_STATEMENT );

		statement->as.expression_stmt = expression;

		return statement;
	}
}

ast_node_t *parser_parse_switch_statement( parser_t *parser )
{
	parser_consume( parser );

	if ( !parser_ensure( parser, TOKEN_OPENING_PAREN, "expected opening parenthesis" ) )
		return NULL;

	ast_node_t *expression = parser_parse_expression( parser );

	if ( !parser_ensure( parser, TOKEN_CLOSING_PAREN, "expected closing parenthesis" ) )
		return NULL;

	if ( !parser_ensure( parser, TOKEN_OPENING_BRACKET, "expected opening bracket" ) )
		return NULL;

	// todo: do the actual body parsing
	while ( !parser_is_eof( parser ) && !parser_matches( parser, TOKEN_CLOSING_BRACKET ) )
		parser_consume( parser );

	if ( !parser_ensure( parser, TOKEN_CLOSING_BRACKET, "expected closing bracket" ) )
		return NULL;

	ast_node_t *switch_def = ast_node_new( AST_NODE_SWITCH_STATEMENT );

	switch_def->as.switch_stmt.expression = expression;

	return switch_def;
}

ast_node_t *parser_parse_for_statement( parser_t *parser )
{
	return NULL;
}

ast_node_t *parser_parse_while_statement( parser_t *parser )
{
	parser_consume( parser );

	if ( !parser_ensure( parser, TOKEN_OPENING_PAREN, "expected opening parenthesis" ) )
		return NULL;

	ast_node_t *expression = parser_parse_expression( parser );

	if ( !parser_ensure( parser, TOKEN_CLOSING_PAREN, "expected closing parenthesis" ) )
		return NULL;

	if ( !parser_ensure( parser, TOKEN_OPENING_BRACKET, "expected opening bracket" ) )
		return NULL;

	// todo: do the actual body parsing
	while ( !parser_is_eof( parser ) && !parser_matches( parser, TOKEN_CLOSING_BRACKET ) )
		parser_consume( parser );

	if ( !parser_ensure( parser, TOKEN_CLOSING_BRACKET, "expected closing bracket" ) )
		return NULL;

	ast_node_t *switch_def = ast_node_new( AST_NODE_WHILE_STATEMENT );

	switch_def->as.while_stmt.expression = expression;

	return switch_def;
}

ast_node_t *parser_parse_if_statement( parser_t *parser )
{
	return NULL;
}

ast_node_t *parser_parse_variable_declaration( parser_t *parser )
{
	parser_consume( parser );

	token_t *name_identifier = NULL;
	token_t *type_identifier = NULL;

	ast_node_t *initial_value = NULL;

	if ( ( name_identifier = parser_ensure( parser, TOKEN_IDENTIFIER, "expected variable name" ) ) == NULL )
		return NULL;

	if ( !parser_ensure( parser, TOKEN_COLON, "expected colon" ) )
		return NULL;

	if ( ( type_identifier = parser_ensure( parser, TOKEN_IDENTIFIER, "expected variable type" ) ) == NULL )
		return NULL;

	if ( parser_expect( parser, TOKEN_OPERATOR_ASSIGN ) )
	{
		initial_value = parser_parse_expression( parser );

		if ( initial_value == NULL )
			return NULL;
	}

	if ( !parser_ensure( parser, TOKEN_SEMICOLON, "expected semicolon" ) )
	{
		if ( initial_value != NULL )
			free( initial_value );

		return NULL;
	}

	ast_node_t *variable_def = ast_node_new( AST_NODE_VARIABLE_DECLARATION );

	variable_def->as.variable_def.identifier = name_identifier;
	variable_def->as.variable_def.type = type_identifier;
	variable_def->as.variable_def.initial_value = initial_value;

	return variable_def;
}

ast_node_t *parser_parse_function_declaration( parser_t *parser )
{
	parser_consume( parser );

	token_t *name_identifier = NULL;

	if ( ( name_identifier = parser_ensure( parser, TOKEN_IDENTIFIER, "expected function name" ) ) == NULL )
		return NULL;

	if ( !parser_ensure( parser, TOKEN_OPENING_PAREN, "expected argument list" ) )
		return NULL;

	// todo: do the actual argument list parsing
	while ( !parser_is_eof( parser ) && !parser_matches( parser, TOKEN_CLOSING_PAREN ) )
		parser_consume( parser );

	if ( !parser_ensure( parser, TOKEN_CLOSING_PAREN, "expected closing parenthesis" ) )
		return NULL;

	if ( !parser_ensure( parser, TOKEN_OPENING_BRACKET, "expected opening bracket" ) )
		return NULL;

	// todo: do the actual body parsing
	while ( !parser_is_eof( parser ) && !parser_matches( parser, TOKEN_CLOSING_BRACKET ) )
		parser_consume( parser );

	if ( !parser_ensure( parser, TOKEN_CLOSING_BRACKET, "expected closing bracket" ) )
		return NULL;

	ast_node_t *function_def = ast_node_new( AST_NODE_FUNCTION_DECLARATION );

	function_def->as.function_def.identifier = name_identifier;
	function_def->as.function_def.return_type = NULL;
	function_def->as.function_def.argument_arity = 0;

	return function_def;
}