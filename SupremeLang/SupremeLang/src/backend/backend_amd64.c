#include <stdint.h>
#include "../../inc/backend/backend_shared.h"
#include "../../inc/backend/backend_amd64.h"
#include "../../inc/keystone/keystone.h"

e_backend_error amd64_assembleXX( int x64, char *code, uint8_t **encoded, size_t *size )
{
	ks_err err = KS_ERR_ARCH;
	ks_engine *ks;
	size_t stat_count;

	//
	// Initialize the architecture
	//

	err = ks_open( KS_ARCH_X86, x64 ? KS_MODE_64 : KS_MODE_32, &ks );

	if ( err != KS_ERR_OK )
	{
		printf( "backend error; ks_open failed!\n" );

		return BACKEND_AMD64_FAILED_KS_OPEN;
	}

	//
	// Assemble the code using our ks_engine. Third argument is zero because it's unneeded on x86.
	//

	if ( ks_asm( ks, code, 0, encoded, size, &stat_count ) != 0 )
	{
		printf(
			"backend error; ks_asm failed! stat_count=%llu, error=%u\n",
			stat_count,
			ks_errno( ks ) );

		ks_close( ks );

		return BACKEND_AMD64_FAILED_KS_ASM;
	}

	ks_close( ks );

	return BACKEND_SUCCESS;
}

e_backend_error amd64_assemble32( char *code, uint8_t **encoded, size_t *size )
{
	return amd64_assembleXX( 0, code, encoded, size );
}

e_backend_error amd64_assemble64( char *code, uint8_t **encoded, size_t *size )
{
	return amd64_assembleXX( 1, code, encoded, size );
}

void amd64_free_output( uint8_t *encoded )
{
	ks_free( encoded );
}