#ifndef SUPREMELANG_LEXER_ERROR_H
#define SUPREMELANG_LEXER_ERROR_H

#include <stdbool.h>

typedef struct _lexer_error_t
{
	bool has_error;

	int32_t line_number;
	int32_t column_number;

	const char *span_start;
	const char *span_end;
	const char *description;
} lexer_error_t;

#endif // SUPREMELANG_LEXER_ERROR_H
