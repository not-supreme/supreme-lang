#include <stdlib.h>

#include "../inc/lexer.h"

bool is_alphabetic( char ch )
{
	return ( ch >= 'A' && ch <= 'Z' ) || ( ch >= 'a' && ch <= 'z' );
}

bool is_numeric( char ch )
{
	return ch >= '0' && ch <= '9';
}

bool is_alphanumeric( char ch )
{
	return is_alphabetic( ch ) || is_numeric( ch );
}

void lexer_init( lexer_t *lexer, char *source_code )
{
	lexer->line_number = lexer->column_number = 0;
	lexer->source = source_code;
	lexer->start = NULL;
}

bool lexer_is_eof( lexer_t *lexer )
{
	return *lexer->source == '\x00';
}

bool lexer_match_next( lexer_t *lexer, char ch )
{
	if ( lexer_is_eof( lexer ) )
		return false;

	bool matches = lexer->source[ 1 ] == ch;

	if ( matches )
		lexer_consume( lexer );

	return matches;
}

char lexer_peek( lexer_t *lexer )
{
	return *lexer->source;
}

char lexer_peek_offset( lexer_t *lexer, int offset )
{
	return lexer->source[ offset ];
}

char lexer_consume( lexer_t *lexer )
{
	char ch = *lexer->source;

	lexer->source++;
	lexer->column_number++;

	return ch;
}

void lexer_handle_whitespace( lexer_t *lexer )
{
	while ( !lexer_is_eof( lexer ) )
	{
		char ch = lexer_peek( lexer );

		switch ( ch )
		{
		case '\r':
		case '\t':
		case ' ':
			lexer->column_number++;
			break;

		case '\n':
			lexer->line_number++;
			lexer->column_number = 0;
			break;

		default:
			return;

		}

		lexer->source++;
	}
}

token_t lexer_simple_token( lexer_t *lexer, e_token_type token_type )
{
	token_t token =
	{
		.token_type = token_type,
		.line_number = lexer->line_number,
		.column_number = lexer->column_number,
		.span_start = lexer->start,
		.span_end = lexer->start,
	};

	lexer_consume( lexer );

	return token;
}

token_t lexer_assign_operator( lexer_t* lexer, e_token_type if_true, e_token_type if_false )
{
	return lexer_simple_token( lexer, lexer_match_next( lexer, '=' ) ? if_true : if_false );
}

token_t lexer_handle_comment( lexer_t *lexer )
{
	token_t token = { .token_type = TOKEN_SKIP };

	lexer_consume( lexer );

	while ( lexer_peek( lexer ) != '\n' )
		lexer_consume( lexer );

	return token;
}

token_t lexer_handle_identifier( lexer_t *lexer )
{
	token_t token =
	{
		.token_type = TOKEN_IDENTIFIER,
		.line_number = lexer->line_number,
		.column_number = lexer->column_number,
		.span_start = lexer->start,
		.span_end = lexer->start,
	};

	lexer_consume( lexer );

	while ( !lexer_is_eof( lexer ) )
	{
		char ch = lexer_peek( lexer );

		if ( !is_alphanumeric( ch ) )
			break;

		lexer_consume( lexer );
	}

	token.span_end = lexer->source;

	token_check_keyword( &token );

	return token;
}

token_t lexer_handle_number( lexer_t *lexer )
{
	bool is_floating_point = false;

	token_t token =
	{
		.line_number = lexer->line_number,
		.column_number = lexer->column_number,
		.span_start = lexer->start,
		.span_end = lexer->start,
	};

	lexer_consume( lexer );

	while ( !lexer_is_eof( lexer ) )
	{
		char ch = lexer_peek( lexer );

		if ( !is_numeric( ch ) )
		{
			if ( is_floating_point )
				break;

			if ( ch == '.' )
				is_floating_point = true;
			else
				break;
		}
		
		lexer_consume( lexer );
	}

	token.token_type = is_floating_point ? TOKEN_NUMBER : TOKEN_INTEGER;
	token.span_end = lexer->source;

	return token;
}

token_t lexer_scan_token( lexer_t *lexer )
{
	lexer_handle_whitespace( lexer );

	if ( lexer_is_eof( lexer ) )
		return lexer_simple_token( lexer, TOKEN_EOF );

	lexer->start = lexer->source;

	char ch = lexer_peek( lexer );
	char next_ch = lexer_peek_offset( lexer, 1 );

	switch ( ch )
	{
	case '!': return lexer_assign_operator( lexer, TOKEN_OPERATOR_NOT_EQUAL, TOKEN_OPERATOR_NOT );
	case '+': return lexer_assign_operator( lexer, TOKEN_OPERATOR_ADD_ASSIGN, TOKEN_OPERATOR_ADD );
	case '-': return lexer_assign_operator( lexer, TOKEN_OPERATOR_SUB_ASSIGN, TOKEN_OPERATOR_SUB );
	case '/': return lexer_match_next( lexer, '/' ) ?
		lexer_handle_comment( lexer ) : lexer_assign_operator( lexer, TOKEN_OPERATOR_DIV_ASSIGN, TOKEN_OPERATOR_DIV );
	case '*': return lexer_assign_operator( lexer, TOKEN_OPERATOR_MUL_ASSIGN, TOKEN_OPERATOR_MUL );
	case '%': return lexer_assign_operator( lexer, TOKEN_OPERATOR_REM_ASSIGN, TOKEN_OPERATOR_REM );
	case '<': return lexer_assign_operator( lexer, TOKEN_OPERATOR_LESS_THAN_EQUAL, TOKEN_OPERATOR_LESS_THAN );
	case '>': return lexer_assign_operator( lexer, TOKEN_OPERATOR_GREATER_THAN_EQUAL, TOKEN_OPERATOR_GREATER_THAN );
	case '=': return lexer_assign_operator( lexer, TOKEN_OPERATOR_EQUAL, TOKEN_OPERATOR_ASSIGN );

	case '(': return lexer_simple_token( lexer, TOKEN_OPENING_PAREN );
	case ')': return lexer_simple_token( lexer, TOKEN_CLOSING_PAREN );
	case '{': return lexer_simple_token( lexer, TOKEN_OPENING_BRACKET );
	case '}': return lexer_simple_token( lexer, TOKEN_CLOSING_BRACKET );
	case '[': return lexer_simple_token( lexer, TOKEN_OPENING_SQUARE_BRACKET );
	case ']': return lexer_simple_token( lexer, TOKEN_CLOSING_SQUARE_BRACKET );
	}

	if ( is_alphabetic( ch ) )
		return lexer_handle_identifier( lexer );

	if ( is_numeric( ch ) )
		return lexer_handle_number( lexer );

	return lexer_simple_token( lexer, TOKEN_UNKNOWN );
}
