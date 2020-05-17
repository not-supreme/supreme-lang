#include "../../inc/parser/parser.h"

ast_node_t *parser_parse_expression( parser_t *parser )
{
    return parser_parse_equality( parser );
}

ast_node_t *parser_parse_equality( parser_t *parser )
{
    ast_node_t *expr = parser_parse_comparison( parser );

    while ( parser_matches( parser, TOKEN_OPERATOR_EQUAL ) || parser_matches( parser, TOKEN_OPERATOR_NOT_EQUAL ) )
    {
        ast_node_t *node = ast_node_new( AST_NODE_BINARY_EXPRESSION );

        node->as.binary_expr.op_token = parser->current_token;

        parser_consume( parser );

        node->as.binary_expr.left = expr;
        node->as.binary_expr.right = parser_parse_comparison( parser );

        expr = node;
    }

    return expr;
}

ast_node_t *parser_parse_comparison( parser_t *parser )
{
    ast_node_t *expr = parser_parse_addition( parser );

    while ( parser_matches( parser, TOKEN_OPERATOR_GREATER_THAN )
        || parser_matches( parser, TOKEN_OPERATOR_GREATER_THAN_EQUAL )
        || parser_matches( parser, TOKEN_OPERATOR_LESS_THAN )
        || parser_matches( parser, TOKEN_OPERATOR_LESS_THAN_EQUAL ) )
    {
        ast_node_t *node = ast_node_new( AST_NODE_BINARY_EXPRESSION );

        node->as.binary_expr.op_token = parser->current_token;

        parser_consume( parser );

        node->as.binary_expr.left = expr;
        node->as.binary_expr.right = parser_parse_addition( parser );

        expr = node;
    }

    return expr;
}

ast_node_t *parser_parse_addition( parser_t *parser )
{
    ast_node_t *expr = parser_parse_multiplication( parser );

    while ( parser_matches( parser, TOKEN_OPERATOR_ADD )
        || parser_matches( parser, TOKEN_OPERATOR_SUB )
        || parser_matches( parser, TOKEN_OPERATOR_REM ) )
    {
        ast_node_t *node = ast_node_new( AST_NODE_BINARY_EXPRESSION );

        node->as.binary_expr.op_token = parser->current_token;

        parser_consume( parser );

        node->as.binary_expr.left = expr;
        node->as.binary_expr.right = parser_parse_multiplication( parser );

        expr = node;
    }

    return expr;
}

ast_node_t *parser_parse_multiplication( parser_t *parser )
{
    ast_node_t *expr = parser_parse_unary( parser );

    while ( parser_matches( parser, TOKEN_OPERATOR_DIV ) || parser_matches( parser, TOKEN_OPERATOR_MUL ) )
    {
        ast_node_t *node = ast_node_new( AST_NODE_BINARY_EXPRESSION );

        node->as.binary_expr.op_token = parser->current_token;

        parser_consume( parser );

        node->as.binary_expr.left = expr;
        node->as.binary_expr.right = parser_parse_unary( parser );

        expr = node;
    }

    return expr;
}

ast_node_t *parser_parse_unary( parser_t *parser )
{
    if ( parser_matches( parser, TOKEN_OPERATOR_NOT ) || parser_matches( parser, TOKEN_OPERATOR_SUB ) )
    {
        ast_node_t *node = ast_node_new( AST_NODE_UNARY_EXPRESSION );

        node->as.unary_expr.op_token = parser->current_token;

        parser_consume( parser );

        node->as.unary_expr.operand = parser_parse_unary( parser );

        return node;
    }

    return parser_parse_primary( parser );
}

ast_node_t *parser_parse_primary( parser_t *parser )
{
    if ( token_type_is_literal( parser->current_token->token_type ) )
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

        //  fixme: report error
        if ( !parser_matches( parser, TOKEN_CLOSING_PAREN ) )
            return NULL;

        node->as.group_expression = expression_node;

        parser_consume( parser );

        return node;
    }

    return NULL;
}
