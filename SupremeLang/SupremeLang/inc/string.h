#ifndef SUPREMELANG_STRING_H
#define SUPREMELANG_STRING_H

#include <stdbool.h>
#include <stdint.h>

typedef struct _intern_entry_t
{
	uint64_t string_hash;
	const char *string_pointer;
} intern_entry_t;

typedef struct _string_interner_t
{
	bool initialized;
	int length;
	int capacity;
	intern_entry_t *entries;
} string_interner_t;

bool check_substring_equality( const char *substring_start, const char *substring_end, const char *check_against );

intern_entry_t *intern_string( const char *string );
intern_entry_t *intern_substring( const char *substring_start, const char *substring_end );

#endif // SUPREMELANG_STRING_H
