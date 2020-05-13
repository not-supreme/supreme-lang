#ifndef SUPREMELANG_LEXER_H
#define SUPREMELANG_LEXER_H

#include "token.h"

typedef struct _lexer_t
{
	int line_number;
	int column_number;

	const char *source;
	const char *start;
	const char *current;
} lexer_t;

void init_lexer( lexer_t *lexer, const char *source_code );

token_t lexer_peek_one( lexer_t *lexer );
token_t lexer_scan_one( lexer_t *lexer );

#endif // SUPREMELANG_LEXER_H
