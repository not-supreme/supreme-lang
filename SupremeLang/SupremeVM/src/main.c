#include "../inc/shared.h"
#include "../inc/cpu.h"
#include "../inc/svm.h"

int main( int argc, char **argv )
{
	/*
	if ( argc < 2 )
	{
		printf( "Invalid usage!\n" );

		return 1;
	}

	return svm_run_from_file( argv[ 1 ] );
	*/
	return svm_run_test( );
}