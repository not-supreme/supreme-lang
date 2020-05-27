#ifndef SL_LEXER_H
#define SL_LEXER_H

typedef const char *sl_lexer_error_t;

typedef struct _sl_lexer_t
{
	const char *source_code;
	const char *token_start;
	const char *current_ch;

	uint32_t line_number;
	uint32_t column_number;
} sl_lexer_t;

/* Initializes the lexer structure.
   Always returns SL_RES_OK. */
sl_result_t sl_lexer_init( sl_lexer_t *lexer );

/* Processes the given source code string.
   Allocates a buffer for the resulting tokens. */
sl_result_t sl_lexer_digest( sl_lexer_t *lexer, const char *const source_code,
	sl_token_t **out_token_buf, int32_t *token_count, sl_lexer_error_t *error_out );

/* Prints the gives lexer error.
   Always returns SL_RES_OK. */
sl_result_t sl_lexer_error_print( sl_lexer_t *lexer, sl_lexer_error_t *error );

#endif // SL_LEXER_H
