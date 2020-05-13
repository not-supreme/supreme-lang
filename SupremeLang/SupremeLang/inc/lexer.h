#ifndef SUPREMELANG_LEXER_H
#define SUPREMELANG_LEXER_H

#include "token.h"

typedef struct _Lexer
{
	int line_number;
	int column_number;

	const char *start;
	const char *current;
} Lexer;

void init_lexer( Lexer *lexer, const char *source_code );

Token lexer_peek_one( Lexer *lexer );
Token lexer_scan_one( Lexer *lexer );

#endif // SUPREMELANG_LEXER_H
