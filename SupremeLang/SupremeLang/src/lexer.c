#include <stdlib.h>

#include "../inc/lexer.h"

void init_lexer( Lexer *lexer, const char *source_code )
{
	lexer->line_number = 0;
	lexer->column_number = 0;

	lexer->current = NULL;
	lexer->start = source_code;
}

Token lexer_peek_one( Lexer *lexer )
{
	Token token =
	{
		.token_type = TOKEN_EOF,
		.span_start = NULL,
		.span_end = NULL
	};

	// read a token

	return token;
}

Token lexer_scan_one( Lexer *lexer )
{
	Token token =
	{
		.token_type = TOKEN_EOF,
		.span_start = NULL,
		.span_end = NULL
	};

	// read a token

	return token;
}
