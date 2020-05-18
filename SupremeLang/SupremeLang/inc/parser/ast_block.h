#ifndef SUPREMELANG_AST_BLOCK_H
#define SUPREMELANG_AST_BLOCK_H

typedef struct _ast_block_t
{
	int length;
	int capacity;

	struct _ast_node_t *nodes;
} ast_block_t;

void ast_block_init( ast_block_t *block );
void ast_block_free( ast_block_t *block );

void ast_block_add( ast_block_t *block, struct _ast_node_t *node );

#endif // SUPREMELANG_AST_BLOCK_H
