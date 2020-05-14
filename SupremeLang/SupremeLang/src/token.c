#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "../inc/string.h"
#include "../inc/token.h"

#define TOKEN_TO_STRING( token ) case TOKEN_##token: return #token;

#define TOKEN_KEYWORD_CHECK( keyword, _token_type )									\
	if ( check_substring_equality( token->span_start, token->span_end, keyword ) )	\
	{																				\
		token->token_type = _token_type;											\
		return;																		\
	}

const char *token_type_to_string( e_token_type token_type )
{
	switch ( token_type )
	{
		//	in-place operators
		TOKEN_TO_STRING( OPERATOR_NOT );
		TOKEN_TO_STRING( OPERATOR_ADD );
		TOKEN_TO_STRING( OPERATOR_SUB );
		TOKEN_TO_STRING( OPERATOR_DIV );
		TOKEN_TO_STRING( OPERATOR_MUL );
		TOKEN_TO_STRING( OPERATOR_REM );
		TOKEN_TO_STRING( OPERATOR_LESS_THAN );
		TOKEN_TO_STRING( OPERATOR_LESS_THAN_EQUAL );
		TOKEN_TO_STRING( OPERATOR_GREATER_THAN );
		TOKEN_TO_STRING( OPERATOR_GREATER_THAN_EQUAL );
		TOKEN_TO_STRING( OPERATOR_EQUAL );
		TOKEN_TO_STRING( OPERATOR_NOT_EQUAL );
		TOKEN_TO_STRING( OPERATOR_ASSIGN );

		//	assignment operators
		TOKEN_TO_STRING( OPERATOR_ADD_ASSIGN );
		TOKEN_TO_STRING( OPERATOR_SUB_ASSIGN );
		TOKEN_TO_STRING( OPERATOR_DIV_ASSIGN );
		TOKEN_TO_STRING( OPERATOR_MUL_ASSIGN );
		TOKEN_TO_STRING( OPERATOR_REM_ASSIGN );

		//	single character tokens
		TOKEN_TO_STRING( OPENING_PAREN );
		TOKEN_TO_STRING( CLOSING_PAREN );
		TOKEN_TO_STRING( OPENING_BRACKET );
		TOKEN_TO_STRING( CLOSING_BRACKET );
		TOKEN_TO_STRING( OPENING_SQUARE_BRACKET );
		TOKEN_TO_STRING( CLOSING_SQUARE_BRACKET );

		//	keywords
		TOKEN_TO_STRING( KEYWORD_FN );
		TOKEN_TO_STRING( KEYWORD_VAR );
		TOKEN_TO_STRING( KEYWORD_TRUE );
		TOKEN_TO_STRING( KEYWORD_FALSE );
		TOKEN_TO_STRING( KEYWORD_SWITCH );
		TOKEN_TO_STRING( KEYWORD_FOR );
		TOKEN_TO_STRING( KEYWORD_WHILE );
		TOKEN_TO_STRING( KEYWORD_IF );
		TOKEN_TO_STRING( KEYWORD_CONTINUE );
		TOKEN_TO_STRING( KEYWORD_BREAK );
		TOKEN_TO_STRING( KEYWORD_RETURN );

		//	misc
		TOKEN_TO_STRING( SKIP );
		TOKEN_TO_STRING( EOF );
		TOKEN_TO_STRING( NUMBER );
		TOKEN_TO_STRING( INTEGER );
		TOKEN_TO_STRING( STRING );
		TOKEN_TO_STRING( IDENTIFIER );
		TOKEN_TO_STRING( UNKNOWN );
	}

	return NULL;
}

int64_t token_get_integer( token_t *token )
{
	char previous_ch = *token->span_end;

	*token->span_end = '\x00';

	int64_t result = strtoll( token->span_start, NULL, 10 );

	*token->span_end = previous_ch;

	return result;
}

double token_get_number( token_t *token )
{
	char previous_ch = *token->span_end;

	*token->span_end = '\x00';

	double result = strtod( token->span_start, NULL );

	*token->span_end = previous_ch;

	return result;
}

const char *token_get_string( token_t *token )
{
	//	todo:
	//	- string interning
	//	- return the actual string
	return NULL;
}

void token_print( token_t *token )
{
	if ( token->token_type == TOKEN_INTEGER )
	{
		printf( "Token: %s (%lld) at %d:%d \n", token_type_to_string( token->token_type ),
			token_get_integer( token ), token->line_number, token->column_number );
	}
	else if ( token->token_type == TOKEN_NUMBER )
	{
		printf( "Token: %s (%.2f) at %d:%d \n", token_type_to_string( token->token_type ),
			token_get_number( token ), token->line_number, token->column_number );
	}
	else if ( token->token_type == TOKEN_STRING )
	{
		printf( "Token: %s (\"%s\") at %d:%d \n", token_type_to_string( token->token_type ),
			token_get_string( token ), token->line_number, token->column_number );
	}
	else
	{
		printf( "Token: %s at %d:%d \n", token_type_to_string( token->token_type ), token->line_number, token->column_number );
	}
}

//	hell :D
void token_check_keyword( token_t *token )
{
	TOKEN_KEYWORD_CHECK( "fn", TOKEN_KEYWORD_FN );
	TOKEN_KEYWORD_CHECK( "var", TOKEN_KEYWORD_VAR );
	TOKEN_KEYWORD_CHECK( "true", TOKEN_KEYWORD_TRUE );
	TOKEN_KEYWORD_CHECK( "false", TOKEN_KEYWORD_FALSE );
	TOKEN_KEYWORD_CHECK( "switch", TOKEN_KEYWORD_SWITCH );
	TOKEN_KEYWORD_CHECK( "for", TOKEN_KEYWORD_FOR );
	TOKEN_KEYWORD_CHECK( "while", TOKEN_KEYWORD_WHILE );
	TOKEN_KEYWORD_CHECK( "if", TOKEN_KEYWORD_IF );
	TOKEN_KEYWORD_CHECK( "continue", TOKEN_KEYWORD_CONTINUE );
	TOKEN_KEYWORD_CHECK( "break", TOKEN_KEYWORD_BREAK );
	TOKEN_KEYWORD_CHECK( "return", TOKEN_KEYWORD_RETURN );
}
