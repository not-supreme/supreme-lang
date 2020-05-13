#include <stdlib.h>

#include "../inc/lexer.h"

void lexer_init( lexer_t *lexer, const char *source_code )
{
	lexer->line_number = lexer->column_number = 0;
	lexer->source = source_code;
	lexer->current = lexer->start = NULL;
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

token_t lexer_scan_token( lexer_t *lexer )
{
	lexer_handle_whitespace( lexer );

	if ( lexer_is_eof( lexer ) )
		return lexer_simple_token( lexer, TOKEN_EOF );

	lexer->start = lexer->current = lexer->source;

	char ch = lexer_peek( lexer );

	switch ( ch )
	{
	case '+': return lexer_simple_token( lexer, TOKEN_OPERATOR_ADD );
	case '-': return lexer_simple_token( lexer, TOKEN_OPERATOR_SUB );
	case '/': return lexer_simple_token( lexer, TOKEN_OPERATOR_DIV );
	case '*': return lexer_simple_token( lexer, TOKEN_OPERATOR_MUL );
	}

	return lexer_simple_token( lexer, TOKEN_UNKNOWN );
}
