#include <stddef.h>

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
		TOKEN_TO_STRING( OPERATOR_ADD );
		TOKEN_TO_STRING( OPERATOR_SUB );
		TOKEN_TO_STRING( OPERATOR_DIV );
		TOKEN_TO_STRING( OPERATOR_MUL );

		//	keywords
		TOKEN_TO_STRING( KEYWORD_FN );
		TOKEN_TO_STRING( KEYWORD_VAR );

		//	misc
		TOKEN_TO_STRING( IDENTIFIER );
		TOKEN_TO_STRING( UNKNOWN );
		TOKEN_TO_STRING( EOF );
	}

	return NULL;
}

//	hell :D
void token_check_keyword( token_t *token )
{
	TOKEN_KEYWORD_CHECK( "fn", TOKEN_KEYWORD_FN );
	TOKEN_KEYWORD_CHECK( "var", TOKEN_KEYWORD_VAR );
}
