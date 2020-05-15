#include <stdlib.h>
#include <string.h>

#include "../inc/string.h"

#define INTERNER_MAX_LOAD 0.75
#define GROW_CAPACITY( capacity ) ( ( capacity ) < 8 ? 8 : ( capacity ) * 2 )

#define FNV1A_BASIS 0xCBF29CE484222325
#define FNV1A_PRIME 0x100000001B3

//	no need for it to be globally defined
//	will only be used by those 2 functions
//	that are implemnted at the bottom
string_interner_t string_interner;

void string_interner_init( string_interner_t *interner )
{
	interner->initialized = true;
	interner->length = 0;
	interner->capacity = -1;
	interner->entries = NULL;
}

void string_interner_adjust_capacity( string_interner_t *interner )
{
	if ( ( int64_t )interner->length + 1 > ( ( int64_t )interner->capacity + 1 ) * INTERNER_MAX_LOAD )
	{
		int capacity = GROW_CAPACITY( interner->capacity + 1 ) - 1;

		intern_entry_t *new_entries = realloc( interner->entries, sizeof( intern_entry_t ) * capacity );

		//	fixme: dont exit silently, report an error instead!
		if ( new_entries == NULL )
			exit( 1 );

		interner->capacity = capacity;
		interner->entries = new_entries;
	}
}

intern_entry_t *string_interner_find_string( string_interner_t *interner, uint64_t string_hash )
{
	for ( int i = 0; i < interner->length; i++ )
	{
		intern_entry_t *entry = &interner->entries[ i ];

		if ( entry->string_hash == string_hash )
			return entry;
	}

	return NULL;
}

intern_entry_t *string_interner_add_string( string_interner_t *interner, uint64_t string_hash, const char *string_pointer )
{
	string_interner_adjust_capacity( interner );

	intern_entry_t *entry = &interner->entries[ interner->length++ ];

	entry->string_hash = string_hash;
	entry->string_pointer = string_pointer;

	return entry;
}

bool check_substring_equality( const char *substring_start, const char *substring_end, const char *check_against )
{
	//	really bad
	int substring_length = ( int )( ( intptr_t )substring_end - ( intptr_t )substring_start );
	int check_against_length = ( int )strlen( check_against );

	if ( substring_length != check_against_length )
		return false;

	for ( int i = 0; i < check_against_length; i++ )
	{
		if ( substring_start[ i ] != check_against[ i ] )
			return false;
	}

	return true;
}

uint64_t hash_string( const char *string, int length )
{
	const char *pointer = string;

	uint64_t hash = FNV1A_BASIS;

	while ( pointer < string + length )
	{
		hash ^= ( uint64_t )*pointer++;
		hash *= FNV1A_PRIME;
	}

	return hash;
}

intern_entry_t *intern_string( const char *string )
{
	char *buffer = malloc( strlen( string ) );

	memset( buffer, NULL, strlen( string ) + 1 );
	memcpy( buffer, string, strlen( string ) );

	uint64_t string_hash = hash_string( buffer, strlen( string ) );

	intern_entry_t *entry = string_interner_find_string( &string_interner, string_hash );

	if ( entry != NULL )
		return entry;

	entry = string_interner_add_string( &string_interner, string_hash, buffer );

	return entry;
}

intern_entry_t *intern_substring( const char *substring_start, const char *substring_end )
{
	int substring_length = ( int )( ( intptr_t )substring_end - ( intptr_t )substring_start );
	char *buffer = malloc( substring_length );

	memset( buffer, NULL, ( size_t )substring_length + 1 );
	memcpy( buffer, substring_start, substring_length );

	uint64_t string_hash = hash_string( buffer, substring_length );

	intern_entry_t *entry = string_interner_find_string( &string_interner, string_hash );

	if ( entry != NULL )
		return entry;

	entry = string_interner_add_string( &string_interner, string_hash, buffer );

	return entry;
}
