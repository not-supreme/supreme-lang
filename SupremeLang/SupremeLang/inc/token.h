#ifndef SUPREMELANG_TOKEN_H
#define SUPREMELANG_TOKEN_H

typedef enum _TokenType
{
	TOKEN_EOF
} TokenType;

typedef struct _Token
{
	TokenType token_type;

	const char *span_start;
	const char *span_end;
} Token;

#endif // SUPREMELANG_TOKEN_H
