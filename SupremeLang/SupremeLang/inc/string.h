#ifndef SUPREMELANG_STRING_H
#define SUPREMELANG_STRING_H

#include <stdbool.h>
#include <stdint.h>

bool check_substring_equality( const char *substring_start, const char *substring_end, const char *check_against );

int64_t intern_string( const char *string );

const char *get_interned_string( int64_t handle );

#endif // SUPREMELANG_STRING_H
