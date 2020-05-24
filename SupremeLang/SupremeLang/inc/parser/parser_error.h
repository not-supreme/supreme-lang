#ifndef SUPREMELANG_PARSER_ERROR_H
#define SUPREMELANG_PARSER_ERROR_H

#include <stdbool.h>

typedef struct _parser_error_t
{
	bool has_error;

	int32_t line_number;
	int32_t column_number;

	const char *description;
} parser_error_t;

#endif // SUPREMELANG_PARSER_ERROR_H
