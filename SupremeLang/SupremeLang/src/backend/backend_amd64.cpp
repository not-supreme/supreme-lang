#include <stdbool.h>
#include <stdint.h>

#include "../../inc/asmjit/asmjit.h"
#include "../../inc/backend/backend_shared.h"

using namespace asmjit;

typedef int( *test_fn_t )( );

extern "C" e_backend_error amd64_assembleXX( bool x64, char *code, uint8_t **encoded, size_t *size )
{
	JitRuntime runtime;
	CodeHolder code_holder;

	code_holder.init( runtime.codeInfo( ) );

	x86::Assembler assembler( &code_holder );

	assembler.mov( x86::rdi, (uintptr_t) "Hello, world!\n" );
	assembler.call( ( uintptr_t ) printf );

	assembler.mov( x86::eax, 1 );
	assembler.ret( );

	CodeBuffer &buffer = code_holder.textSection( )->buffer( );

	for ( size_t i = 0; i < buffer.size( ); i++ )
		printf( "%02X ", buffer.data( )[ i ] );

	return BACKEND_SUCCESS;
}

extern "C" e_backend_error amd64_assemble32( char *code, uint8_t **encoded, size_t *size )
{
	return amd64_assembleXX( false, code, encoded, size );
}

extern "C" e_backend_error amd64_assemble64( char *code, uint8_t **encoded, size_t *size )
{
	return amd64_assembleXX( true, code, encoded, size );
}
