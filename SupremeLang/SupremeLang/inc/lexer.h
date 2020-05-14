#ifndef SUPREMELANG_LEXER_H
#define SUPREMELANG_LEXER_H

#include <stdbool.h>

#include "token.h"

typedef struct _lexer_t
{
	int line_number;
	int column_number;

	char *source;
	char *start;
} lexer_t;

void lexer_init( lexer_t *lexer, char *source_code );

bool lexer_is_eof( lexer_t *lexer );
bool lexer_match_next( lexer_t *lexer, char ch );

char lexer_peek( lexer_t *lexer );
char lexer_peek_offset( lexer_t *lexer, int offset );
char lexer_consume( lexer_t *lexer );

token_t lexer_scan_token( lexer_t *lexer );

#endif // SUPREMELANG_LEXER_H
