#include <stdlib.h>

#include "../inc/lexer.h"

void init_lexer( lexer_t *lexer, const char *source_code )
{
	lexer->line_number = lexer->column_number = 0;
	lexer->source = source_code;
	lexer->current = lexer->start = NULL;
}

token_t lexer_single_char_token( lexer_t *lexer, e_token_type token_type )
{
	token_t token =
	{
		.token_type = token_type,
		.span_start = lexer->start,
		.span_end = lexer->start,
	};

	lexer->source++;

	return token;
}

token_t lexer_end_of_input( lexer_t *lexer )
{
	token_t token =
	{
		.token_type = TOKEN_EOF,
		.span_start = lexer->source,
		.span_end = lexer->source,
	};

	return token;
}

token_t lexer_unknown_token( lexer_t *lexer )
{
	token_t token =
	{
		.token_type = TOKEN_UNKNOWN,
		.span_start = lexer->start,
		.span_end = lexer->start,
	};

	// TODO: Print an error message?

	lexer->source++;

	return token;
}

token_t lexer_peek_one( lexer_t *lexer )
{
	token_t token =
	{
		.token_type = TOKEN_EOF,
		.span_start = NULL,
		.span_end = NULL
	};

	// read a token

	return token;
}

token_t lexer_scan_one( lexer_t *lexer )
{
	if ( *lexer->source == '\x00' )
		return lexer_end_of_input( lexer );

	lexer->start = lexer->current = lexer->source;

	char ch = lexer->source[ 0 ];

	switch ( ch )
	{
	case '+': return lexer_single_char_token( lexer, TOKEN_OPERATOR_ADD );
	case '-': return lexer_single_char_token( lexer, TOKEN_OPERATOR_SUB );
	case '/': return lexer_single_char_token( lexer, TOKEN_OPERATOR_DIV );
	case '*': return lexer_single_char_token( lexer, TOKEN_OPERATOR_MUL );
	}

	return lexer_unknown_token( lexer );
}
