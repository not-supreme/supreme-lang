#ifndef SUPREMELANG_TOKEN_H
#define SUPREMELANG_TOKEN_H

typedef enum _e_token_type
{
	// Operators
	TOKEN_OPERATOR_ADD,
	TOKEN_OPERATOR_SUB,
	TOKEN_OPERATOR_DIV,
	TOKEN_OPERATOR_MUL,

	// Misc
	TOKEN_UNKNOWN,
	TOKEN_EOF
} e_token_type;

const char *token_type_to_string( e_token_type token_type );

typedef struct _token_t
{
	e_token_type token_type;

	int line_number;
	int column_number;

	const char *span_start;
	const char *span_end;
} token_t;

#endif // SUPREMELANG_TOKEN_H
