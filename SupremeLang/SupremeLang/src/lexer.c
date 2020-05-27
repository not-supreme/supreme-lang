#include "../inc/shared.h"
#include "../inc/result.h"
#include "../inc/token.h"
#include "../inc/lexer.h"

#define SIMPLE_TOKEN( ch, token_type ) \
	case ch: SL_ASSERT( sl_create_simple_token( lexer, next_token, token_type ) == SL_RES_OK ); break;

#define ASSIGN_TOKEN( ch, token_if_true, token_if_false )											\
	case ch: {																						\
		if ( *( lexer->current_ch + 1 ) == '=' ) {													\
			SL_ASSERT( sl_create_simple_token( lexer, next_token, token_if_true ) == SL_RES_OK );	\
			lexer->column_number++; lexer->current_ch++;											\
		} else {																					\
			SL_ASSERT( sl_create_simple_token( lexer, next_token, token_if_false ) == SL_RES_OK );	\
		}																							\
	}; break;

#define HANDLE_KEYWORD( keyword, tk_type ) \
	if ( strcmp( token->as.string, keyword ) == 0 ) { token->token_type = tk_type; return SL_RES_OK; }

typedef enum _sl_e_integer_mode_t
{
	INT_BINARY,
	INT_OCTAL,
	INT_DECIMAL,
	INT_HEX
} sl_e_integer_mode_t;

int integer_mode_to_radix( sl_e_integer_mode_t  integer_mode )
{
	switch ( integer_mode )
	{
	case INT_BINARY: return 2;
	case INT_OCTAL: return 8;
	case INT_DECIMAL: return 10;
	case INT_HEX: return 16;
	}

	return -1;
}

bool ch_is_whitespace( char ch )
{
	return ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n';
}

bool ch_is_alphabetic( char ch )
{
	return ( ch >= 'a' && ch <= 'z' ) || ( ch >= 'A' && ch <= 'Z' );
}

bool ch_is_numeric( char ch )
{
	return ch >= '0' && ch <= '9';
}

bool ch_matches_integer_mode( char ch, sl_e_integer_mode_t integer_mode )
{
	switch ( integer_mode )
	{
	case INT_BINARY: return ch == '0' || ch == '1';
	case INT_OCTAL:return ch >= '0' && ch <= '8';
	case INT_DECIMAL: return ch >= '0' && ch <= '9';
	case INT_HEX:  return ( ch >= '0' && ch <= '9' ) || ( ch >= 'a' && ch <= 'f' ) || ( ch >= 'A' && ch <= 'F' );
	}

	SL_ASSERT( true == false );
}

bool ch_is_alphanumeric( char ch )
{
	return ( ch >= 'a' && ch <= 'z' ) || ( ch >= 'A' && ch <= 'Z' ) || ( ch >= '0' && ch <= '9' );
}

sl_result_t reallocate_token_buffer( sl_token_t **out_token_buf, int32_t *capacity )
{
	// Adjust the capacity of the token buffer.
	*capacity = ( ( *capacity ) < 16 ) ? 16 : ( *capacity ) * 2;

	// Check if the token buffer was originally allocated.
	// If not, allocate it with the initial capacity.
	if ( *out_token_buf == NULL )
	{
		*out_token_buf = malloc( sizeof( sl_token_t ) * ( *capacity ) );

		SL_ASSERT( *out_token_buf != NULL );
	}
	// Else, reallocate the existing memory block.
	else
	{
		sl_token_t *new_buffer = realloc( *out_token_buf, sizeof( sl_token_t ) * ( *capacity ) );

		SL_ASSERT( new_buffer != NULL );

		*out_token_buf = new_buffer;
	}

	return SL_RES_OK;
}

sl_result_t sl_create_simple_token( sl_lexer_t *lexer, sl_token_t *token, sl_e_token_type_t token_type )
{
	token->token_type = token_type;
	token->line_number = lexer->line_number;
	token->column_number = lexer->column_number;
	token->as.integer = 0;

	lexer->column_number++;
	lexer->current_ch++;

	return SL_RES_OK;
}

sl_result_t sl_handle_comment( sl_lexer_t *lexer, sl_lexer_error_t *error_out, bool full_line_comment )
{
	bool is_terminated = full_line_comment;

	while ( *lexer->current_ch != '\x00' )
	{
		char ch = *lexer->current_ch;

		lexer->column_number++;
		lexer->current_ch++;

		if ( ch == '\n' )
		{
			lexer->line_number++;
			lexer->column_number = 0;
		}

		if ( full_line_comment && ch == '\n' )
			break;

		if ( !full_line_comment && ch == '*' && *lexer->current_ch == '/' )
		{
			lexer->column_number++;
			lexer->current_ch++;

			is_terminated = true;

			break;
		}
	}

	if ( !is_terminated )
	{
		*error_out = "unterminated comment";

		return SL_RES_LEX_ERROR;
	}

	return SL_RES_OK;
}

sl_result_t sl_handle_identifier( sl_lexer_t *lexer, sl_token_t *token, sl_lexer_error_t *error_out )
{
	while ( *lexer->current_ch != '\x00' && ch_is_alphanumeric( *lexer->current_ch ) )
	{
		lexer->column_number++;
		lexer->current_ch++;
	}

	size_t identifier_length = ( size_t )( lexer->current_ch - lexer->token_start );

	token->token_type = TOKEN_IDENTIFIER;
	token->line_number = lexer->line_number;
	token->column_number = lexer->column_number - ( int32_t )identifier_length;
	token->as.string = malloc( identifier_length + 1 );

	SL_ASSERT( token->as.string != NULL );

	memset( token->as.string, 0, identifier_length + 1 );
	memcpy( token->as.string, lexer->token_start, identifier_length );

	HANDLE_KEYWORD( "fn", TOKEN_KEYWORD_FN );
	HANDLE_KEYWORD( "var", TOKEN_KEYWORD_VAR );
	HANDLE_KEYWORD( "true", TOKEN_KEYWORD_TRUE );
	HANDLE_KEYWORD( "false", TOKEN_KEYWORD_FALSE );
	HANDLE_KEYWORD( "switch", TOKEN_KEYWORD_SWITCH );
	HANDLE_KEYWORD( "for", TOKEN_KEYWORD_FOR );
	HANDLE_KEYWORD( "while", TOKEN_KEYWORD_WHILE );
	HANDLE_KEYWORD( "if", TOKEN_KEYWORD_IF );
	HANDLE_KEYWORD( "continue", TOKEN_KEYWORD_CONTINUE );
	HANDLE_KEYWORD( "break", TOKEN_KEYWORD_BREAK );
	HANDLE_KEYWORD( "return", TOKEN_KEYWORD_RETURN );

	return SL_RES_OK;
}

sl_result_t sl_handle_number_literal( sl_lexer_t *lexer, sl_token_t *token, sl_lexer_error_t *error_out )
{
	bool is_floating_pt = false;

	sl_e_integer_mode_t integer_mode = INT_DECIMAL;

	while ( *lexer->current_ch != '\x00' )
	{
		if ( *lexer->current_ch == '_' )
		{
			lexer->column_number++;
			lexer->current_ch++;

			continue;
		}

		if ( lexer->current_ch == lexer->token_start )
		{
			if ( *lexer->current_ch == '0' && *( lexer->current_ch + 1 ) != '\x00'
				&& *( lexer->current_ch + 1 ) != '.' && *( lexer->current_ch + 1 ) != '_'
				&& !ch_is_numeric( *( lexer->current_ch + 1 ) ) )
			{
				lexer->column_number++;
				lexer->current_ch++;

				switch ( *lexer->current_ch )
				{
				case 'b':
					lexer->column_number++;
					lexer->current_ch++;

					integer_mode = INT_BINARY;
					break;
				case 'o':
					lexer->column_number++;
					lexer->current_ch++;

					integer_mode = INT_OCTAL;
					break;
				case 'x':
					lexer->column_number++;
					lexer->current_ch++;

					integer_mode = INT_HEX;
					break;
				default:
					*error_out = "unknown integer type";

					return SL_RES_LEX_ERROR;
				}

				if ( *lexer->current_ch == '\x00' || !ch_matches_integer_mode( *lexer->current_ch, integer_mode ) )
				{
					*error_out = "incomplete integer literal";

					return SL_RES_LEX_ERROR;
				}
			}
			else if ( ch_matches_integer_mode( *lexer->current_ch, integer_mode ) )
			{
				lexer->column_number++;
				lexer->current_ch++;
			}
			else
			{
				break;
			}
		}
		else if ( ch_matches_integer_mode( *lexer->current_ch, integer_mode ) )
		{
			lexer->column_number++;
			lexer->current_ch++;
		}
		else if ( integer_mode == INT_DECIMAL && !is_floating_pt && *lexer->current_ch == '.' )
		{
			lexer->column_number++;
			lexer->current_ch++;

			is_floating_pt = true;
		}
		else
		{
			break;
		}
	}

	int32_t literal_length = ( int32_t )( lexer->current_ch - lexer->token_start );

	size_t str_literal_offset = integer_mode == INT_DECIMAL ? 0 : 2;
	size_t literal_str_size = ( size_t )literal_length - str_literal_offset;
	size_t size_without_uss = literal_str_size;

	if ( is_floating_pt && literal_str_size == 2 )
	{
		*error_out = "incomplete floating point number";

		return SL_RES_LEX_ERROR;
	}

	for ( size_t i = 0; i < literal_str_size; i++ )
	{
		if ( *( lexer->token_start + i + str_literal_offset ) == '_' )
			size_without_uss--;
	}

	char *literal_str = malloc( size_without_uss + 1 );

	memset( literal_str, 0, size_without_uss + 1 );

	for ( size_t i = 0, j = 0; i < literal_str_size; i++ )
	{
		if ( *( lexer->token_start + i + str_literal_offset ) != '_' )
		{
			literal_str[ j++ ] = *( lexer->token_start + i + str_literal_offset );
		}
	}

	token->token_type = is_floating_pt ? TOKEN_NUMBER : TOKEN_INTEGER;
	token->line_number = lexer->line_number;
	token->column_number = lexer->column_number - literal_length;

	if ( is_floating_pt )
		token->as.number = strtod( literal_str, NULL );
	else
		token->as.integer = strtoull( literal_str, NULL, integer_mode_to_radix( integer_mode ) );

	return SL_RES_OK;
}

sl_result_t sl_handle_string_literal( sl_lexer_t *lexer, sl_token_t *token, sl_lexer_error_t *error_out )
{
	bool is_terminated = false;

	while ( *lexer->current_ch != '\x00' && !is_terminated )
	{
		if ( *lexer->current_ch == '\\' )
		{
			lexer->current_ch++;
			lexer->column_number++;
		}
		else if ( *lexer->current_ch == '"' )
		{
			is_terminated = true;
		}

		lexer->column_number++;
		lexer->current_ch++;
	}

	if ( !is_terminated )
	{
		*error_out = "unterminated string";

		return SL_RES_LEX_ERROR;
	}

	size_t string_length = ( size_t )( lexer->current_ch - lexer->token_start ) - 2;

	token->token_type = TOKEN_STRING;
	token->line_number = lexer->line_number;
	token->column_number = lexer->column_number - ( int32_t )( string_length + 2 );
	token->as.string = malloc( string_length + 1 );

	SL_ASSERT( token->as.string != NULL );

	memset( token->as.string, 0, string_length + 1 );
	memcpy( token->as.string, lexer->token_start + 1, string_length );

	return SL_RES_OK;
}

sl_result_t sl_lexer_init( sl_lexer_t *lexer )
{
	lexer->token_start = NULL;
	lexer->current_ch = NULL;

	lexer->line_number = 0;
	lexer->column_number = 0;

	return SL_RES_OK;
}

sl_result_t sl_lexer_digest( sl_lexer_t *lexer, const char *const source_code,
	sl_token_t **out_token_buf, int32_t *token_count, sl_lexer_error_t *error_out )
{
	// Hold a pointer to the source code we are
	// currently scanning. Used for error printing.
	lexer->source_code = source_code;

	// Variables used to keep track of how many tokens there
	// are and how many tokens the buffer can hold.
	int32_t token_buf_capacity = 1;
	int32_t token_amount = 0;

	// Token buffer. Initialized to null.
	sl_token_t *token_buf = NULL;

	// Allocate the token buffer and make sure that
	// the allocation didn't fail.
	SL_ASSERT( reallocate_token_buffer( &token_buf, &token_buf_capacity ) == SL_RES_OK );
	SL_ASSERT( token_buf != NULL );

	*out_token_buf = token_buf;
	*token_count = token_amount;

	// Set up the `current_ch` member to point to the
	// first possible character in the token.
	lexer->current_ch = source_code;

	// Loop while the stream input is valid.
	while ( *lexer->current_ch != '\x00' )
	{
		char ch = *lexer->current_ch;

		// Skip whitespace if possible.
		if ( ch == ' ' || ch == '\t' || ch == '\r' )
		{
			lexer->current_ch++;
			lexer->column_number++;
		}
		else if ( ch == '\n' )
		{
			lexer->current_ch++;
			lexer->line_number++;
			lexer->column_number = 0;
		}
		else if ( ch == '/' && ( *( lexer->current_ch + 1 ) == '/' || *( lexer->current_ch + 1 ) == '*' ) )
		{
			bool full_line_comment = *( lexer->current_ch + 1 ) == '/';

			sl_result_t parse_result = sl_handle_comment( lexer, error_out, full_line_comment );

			if ( parse_result != SL_RES_OK )
				return parse_result;
		}
		else
		{
			// Set the `token_start` member variable.
			lexer->token_start = lexer->current_ch;

			// Check if we have enough space for tokens.
			// If not, allocate some more memory!
			if ( token_amount >= ( token_buf_capacity * 0.75 ) )
			{
				SL_ASSERT( reallocate_token_buffer( &token_buf, &token_buf_capacity ) == SL_RES_OK );
				SL_ASSERT( token_buf != NULL );

				*out_token_buf = token_buf;
				*token_count = token_amount;
			}

			// Get a pointer to the next available token.
			sl_token_t *next_token = &token_buf[ token_amount++ ];

			if ( ch_is_alphabetic( ch ) )
			{
				sl_result_t parse_result = sl_handle_identifier( lexer, next_token, error_out );

				if ( parse_result != SL_RES_OK )
					return parse_result;
			}
			else if ( ch_is_numeric( ch ) )
			{
				sl_result_t parse_result = sl_handle_number_literal( lexer, next_token, error_out );

				if ( parse_result != SL_RES_OK )
					return parse_result;
			}
			else if ( ch == '"' )
			{
				lexer->current_ch++;
				lexer->column_number++;

				sl_result_t parse_result = sl_handle_string_literal( lexer, next_token, error_out );

				if ( parse_result != SL_RES_OK )
					return parse_result;
			}
			else
			{
				// Handle the first character of the token
				switch ( ch )
				{
					SIMPLE_TOKEN( '(', TOKEN_LEFT_PAREN );
					SIMPLE_TOKEN( ')', TOKEN_RIGHT_PAREN );
					SIMPLE_TOKEN( '{', TOKEN_LEFT_BRACE );
					SIMPLE_TOKEN( '}', TOKEN_RIGHT_BRACE );
					SIMPLE_TOKEN( ';', TOKEN_SEMICOLON );
					SIMPLE_TOKEN( ',', TOKEN_COMMA );
					SIMPLE_TOKEN( '.', TOKEN_DOT );
					SIMPLE_TOKEN( '-', TOKEN_MINUS );
					SIMPLE_TOKEN( '+', TOKEN_PLUS );
					SIMPLE_TOKEN( '/', TOKEN_SLASH );
					SIMPLE_TOKEN( '*', TOKEN_STAR );

					ASSIGN_TOKEN( '!', TOKEN_BANG_EQUAL, TOKEN_BANG );
					ASSIGN_TOKEN( '=', TOKEN_EQUAL_EQUAL, TOKEN_EQUAL );
					ASSIGN_TOKEN( '<', TOKEN_LESS_EQUAL, TOKEN_LESS );
					ASSIGN_TOKEN( '>', TOKEN_GREATER_EQUAL, TOKEN_GREATER );

				default:
					*error_out = "unrecognized token";

					if ( token_buf != NULL )
						free( token_buf );

					return SL_RES_LEX_ERROR;
				}
			}
		}
	}

	// Write the EOF token and advance to the next line.
	token_buf[ token_amount ].token_type = TOKEN_EOF;
	token_buf[ token_amount ].line_number = lexer->line_number;
	token_buf[ token_amount ].column_number = lexer->column_number;

	lexer->line_number++;
	lexer->column_number = 0;

	// Pass the token buffer and token count back to the caller.
	*out_token_buf = token_buf;
	*token_count = token_amount++;

	return SL_RES_OK;
}

sl_result_t sl_lexer_error_print( sl_lexer_t *lexer, sl_lexer_error_t *error )
{
	int32_t line_number = lexer->line_number + 1;
	int32_t pad_size = 0;

	while ( line_number > 0 )
	{
		line_number /= 10;
		pad_size += 1;
	}

	int32_t length = ( int32_t )( lexer->current_ch - lexer->token_start );

	printf( "error: %s \n", *error );
	printf( "at repl:%d:%d \n", lexer->line_number + 1, lexer->column_number + 1 );

	printf( " %*s | \n", pad_size, "" );
	printf( " %-*d | %s \n", pad_size, lexer->line_number + 1, lexer->source_code );
	printf( " %*s | %*s", pad_size, "", lexer->column_number - length, "" );

	for ( int32_t i = 0; i <= max( 1, length ); i++ )
		putchar( i == max( 1, length ) ? '\n' : '^' );

	return SL_RES_OK;
}
