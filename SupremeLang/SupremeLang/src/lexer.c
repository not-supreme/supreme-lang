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

void lexer_init( lexer_t *lexer, const char *source_code )
{
	lexer->line_number = lexer->column_number = 0;
	lexer->source = source_code;
	lexer->start = NULL;
}

bool lexer_is_eof( lexer_t *lexer )
{
	return *lexer->source == '\x00';
}

char lexer_peek( lexer_t *lexer )
{
	return *lexer->source;
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
}

token_t lexer_scan_token( lexer_t *lexer )
{
	lexer_handle_whitespace( lexer );

	if ( lexer_is_eof( lexer ) )
		return lexer_simple_token( lexer, TOKEN_EOF );

	lexer->start = lexer->source;

	char ch = lexer_peek( lexer );

	switch ( ch )
	{
	case '+': return lexer_simple_token( lexer, TOKEN_OPERATOR_ADD );
	case '-': return lexer_simple_token( lexer, TOKEN_OPERATOR_SUB );
	case '/': return lexer_simple_token( lexer, TOKEN_OPERATOR_DIV );
	case '*': return lexer_simple_token( lexer, TOKEN_OPERATOR_MUL );
	}

	if ( is_alphabetic( ch ) )
		return lexer_handle_identifier( lexer );

	if ( is_numeric( ch ) )
		return lexer_handle_number( lexer );

	return lexer_simple_token( lexer, TOKEN_UNKNOWN );
}
