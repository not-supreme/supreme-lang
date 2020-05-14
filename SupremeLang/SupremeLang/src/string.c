#include <stdint.h>
#include <string.h>

#include "../inc/string.h"

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

int64_t intern_string( const char *string )
{
	//	todo: implement that
	return 0;
}

const char *get_interned_string( int64_t handle )
{
	//	todo: implement that
	return NULL;
}
