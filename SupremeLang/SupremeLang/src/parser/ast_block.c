#include <memory.h>
#include <stdlib.h>

#include "../../inc/parser/ast_block.h"
#include "../../inc/parser/ast_node.h"

#define PARSER_MAX_LOAD 0.75
#define GROW_CAPACITY( capacity ) ( ( capacity ) < 8 ? 8 : ( capacity ) * 2 )

void ast_block_init( ast_block_t *block )
{
	block->length = 0;
	block->capacity = -1;
	block->nodes = NULL;
}

void ast_block_free( ast_block_t *block )
{
	if ( block->nodes != NULL )
		free( block->nodes );

	ast_block_init( block );
}

void ast_block_adjust_capacity( ast_block_t *block )
{
	if ( ( int64_t )block->length + 1 > ( ( int64_t )block->capacity + 1 ) * PARSER_MAX_LOAD )
	{
		int32_t capacity = GROW_CAPACITY( block->capacity + 1 ) - 1;

		ast_node_t *new_nodes = realloc( block->nodes, sizeof( ast_node_t ) * capacity );

		// fixme: dont exit silently, report an error instead!

		if ( new_nodes == NULL )
			exit( 1 );

		block->capacity = capacity;
		block->nodes = new_nodes;
	}
}

void ast_block_add( ast_block_t *block, ast_node_t *node )
{
	ast_block_adjust_capacity( block );

	memcpy( &block->nodes[ block->length++ ], node, sizeof( ast_node_t ) );
}
