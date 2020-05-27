#ifndef SL_TOKEN_H
#define SL_TOKEN_H

typedef enum _sl_e_token_type_t
{
    TOKEN_LEFT_PAREN,
    TOKEN_RIGHT_PAREN,
    TOKEN_LEFT_BRACE,
    TOKEN_RIGHT_BRACE,
    TOKEN_COMMA,
    TOKEN_DOT,
    TOKEN_SEMICOLON,
    TOKEN_MINUS,
    TOKEN_PLUS,
    TOKEN_SLASH,
    TOKEN_STAR,

    TOKEN_BANG,
    TOKEN_BANG_EQUAL,
    TOKEN_EQUAL,
    TOKEN_EQUAL_EQUAL,
    TOKEN_GREATER,
    TOKEN_GREATER_EQUAL,
    TOKEN_LESS,
    TOKEN_LESS_EQUAL,
    
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_INTEGER,
    TOKEN_STRING,

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

	TOKEN_SKIP,
	TOKEN_EOF
} sl_e_token_type_t;

static const char *const token_names[ ] =
{
    "TOKEN_LEFT_PAREN",
    "TOKEN_RIGHT_PAREN",
    "TOKEN_LEFT_BRACE",
    "TOKEN_RIGHT_BRACE",
    "TOKEN_COMMA",
    "TOKEN_DOT",
    "TOKEN_SEMICOLON",
    "TOKEN_MINUS",
    "TOKEN_PLUS",
    "TOKEN_SLASH",
    "TOKEN_STAR",

    "TOKEN_BANG",
    "TOKEN_BANG_EQUAL",
    "TOKEN_EQUAL",
    "TOKEN_EQUAL_EQUAL",
    "TOKEN_GREATER",
    "TOKEN_GREATER_EQUAL",
    "TOKEN_LESS",
    "TOKEN_LESS_EQUAL",

    "TOKEN_IDENTIFIER",
    "TOKEN_NUMBER",
    "TOKEN_INTEGER",
    "TOKEN_STRING",

    "TOKEN_KEYWORD_FN",
    "TOKEN_KEYWORD_VAR",
    "TOKEN_KEYWORD_TRUE",
    "TOKEN_KEYWORD_FALSE",
    "TOKEN_KEYWORD_SWITCH",
    "TOKEN_KEYWORD_FOR",
    "TOKEN_KEYWORD_WHILE",
    "TOKEN_KEYWORD_IF",
    "TOKEN_KEYWORD_CONTINUE",
    "TOKEN_KEYWORD_BREAK",
    "TOKEN_KEYWORD_RETURN",

    "TOKEN_SKIP",
    "TOKEN_EOF"
};

typedef struct _sl_token_t
{
	sl_e_token_type_t token_type;

	uint32_t line_number;
	uint32_t column_number;

    union _sl_token_repr_t
    {
        char *string;
        uint64_t integer;
        double number;
    } as;
} sl_token_t;

/* Frees the token structure.
   Always returns SL_RES_OK. */
sl_result_t sl_token_free( sl_token_t *token );

/* Prints the debug representation of a token.
   Always returns SL_RES_OK. */
sl_result_t sl_token_debug_print( sl_token_t *token );

#endif // SL_TOKEN_H
