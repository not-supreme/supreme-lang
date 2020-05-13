#include <stddef.h>

#include "../inc/token.h"

#define TOKEN_TO_STRING( token ) case TOKEN_##token: return #token;

const char *token_type_to_string( e_token_type token_type )
{
	switch ( token_type )
	{
		TOKEN_TO_STRING( OPERATOR_ADD );
		TOKEN_TO_STRING( OPERATOR_SUB );
		TOKEN_TO_STRING( OPERATOR_DIV );
		TOKEN_TO_STRING( OPERATOR_MUL );

		TOKEN_TO_STRING( UNKNOWN );
		TOKEN_TO_STRING( EOF );
	}

	return NULL;
}
