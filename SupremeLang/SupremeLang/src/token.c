#include "../inc/shared.h"
#include "../inc/result.h"
#include "../inc/token.h"

sl_result_t sl_token_free( sl_token_t *token )
{
	if ( token->token_type == TOKEN_IDENTIFIER || token->token_type == TOKEN_STRING && token->as.string != NULL )
	{
		free( token->as.string );

		token->as.string = NULL;
	}

	return SL_RES_OK;
}

sl_result_t sl_token_debug_print( sl_token_t *token )
{
	if ( token->token_type == TOKEN_IDENTIFIER )
	{
		SL_DEBUG_LOG( "got %s (%s) at %d:%d", token_names[ token->token_type ],
			token->as.string, token->line_number + 1, token->column_number + 1 );
	}
	else if ( token->token_type == TOKEN_NUMBER )
	{
		SL_DEBUG_LOG( "got %s (%f) at %d:%d", token_names[ token->token_type ],
			token->as.number, token->line_number + 1, token->column_number + 1 );
	}
	else if ( token->token_type == TOKEN_INTEGER )
	{
		SL_DEBUG_LOG( "got %s (%lld) at %d:%d", token_names[ token->token_type ],
			token->as.integer, token->line_number + 1, token->column_number + 1 );
	}
	else if ( token->token_type == TOKEN_STRING )
	{
		SL_DEBUG_LOG( "got %s (%s) at %d:%d", token_names[ token->token_type ],
			token->as.string, token->line_number + 1, token->column_number + 1 );
	}
	else
	{
		SL_DEBUG_LOG( "got %s at %d:%d", token_names[ token->token_type ],
			token->line_number + 1, token->column_number + 1 );
	}

	return SL_RES_OK;
}
