#ifndef SUPREMELANG_TOKEN_H
#define SUPREMELANG_TOKEN_H

#include <stdbool.h>
#include <stdint.h>

typedef enum _e_token_type
{
	//	in-place operators
	TOKEN_OPERATOR_NOT,
	TOKEN_OPERATOR_ADD,
	TOKEN_OPERATOR_SUB,
	TOKEN_OPERATOR_DIV,
	TOKEN_OPERATOR_MUL,
	TOKEN_OPERATOR_REM,
	TOKEN_OPERATOR_LESS_THAN,
	TOKEN_OPERATOR_LESS_THAN_EQUAL,
	TOKEN_OPERATOR_GREATER_THAN,
	TOKEN_OPERATOR_GREATER_THAN_EQUAL,
	TOKEN_OPERATOR_EQUAL,
	TOKEN_OPERATOR_NOT_EQUAL,
	TOKEN_OPERATOR_ASSIGN,
	
	//	assignment operators
	TOKEN_OPERATOR_ADD_ASSIGN,
	TOKEN_OPERATOR_SUB_ASSIGN,
	TOKEN_OPERATOR_DIV_ASSIGN,
	TOKEN_OPERATOR_MUL_ASSIGN,
	TOKEN_OPERATOR_REM_ASSIGN,

	//	single character tokens
	TOKEN_OPENING_PAREN,
	TOKEN_CLOSING_PAREN,
	TOKEN_OPENING_BRACKET,
	TOKEN_CLOSING_BRACKET,
	TOKEN_OPENING_SQUARE_BRACKET,
	TOKEN_CLOSING_SQUARE_BRACKET,

	//	keywords
	TOKEN_KEYWORD_IMPORT,
	TOKEN_KEYWORD_FN,
	TOKEN_KEYWORD_VAR,
	TOKEN_KEYWORD_TRUE,
	TOKEN_KEYWORD_FALSE,
	TOKEN_KEYWORD_SWITCH,
	TOKEN_KEYWORD_FOR,
	TOKEN_KEYWORD_WHILE,
	TOKEN_KEYWORD_IF,
	TOKEN_KEYWORD_CONTINUE,
	TOKEN_KEYWORD_BREAK,
	TOKEN_KEYWORD_RETURN,

	//	misc
	TOKEN_INTEGER,
	TOKEN_NUMBER,
	TOKEN_STRING,
	TOKEN_IDENTIFIER,
	TOKEN_COLON,
	TOKEN_SEMICOLON,
	TOKEN_UNKNOWN,
	TOKEN_SKIP,
	TOKEN_EOF,
} e_token_type;

bool token_type_is_operator_in_place( e_token_type token_type );
bool token_type_is_operator_assign( e_token_type token_type );
bool token_type_is_operator( e_token_type token_type );
bool token_type_is_keyword( e_token_type token_type );
bool token_type_is_literal( e_token_type token_type );

const char *token_type_to_string( e_token_type token_type );

typedef struct _token_t
{
	e_token_type token_type;

	int line_number;
	int column_number;

	char *span_start;
	char *span_end;
} token_t;

bool token_get_boolean( token_t *token );
int64_t token_get_integer( token_t *token );
double token_get_number( token_t *token );
const char *token_get_string( token_t *token );

void token_print( token_t *token );
void token_check_keyword( token_t *token );

#endif // SUPREMELANG_TOKEN_H
