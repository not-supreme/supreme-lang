#include "../inc/shared.h"
#include "../inc/cpu.h"
#include "../inc/svm.h"

int svm_run_from_file( char *file_path )
{
    FILE *fp;           // File pointer
    long len;           // File length (in bytes)
    char *buf;          // Pointer to file buffer

    fp = fopen( file_path, "rb" );
    if ( !fp )
    {
        printf( "Failed to open file \"%s\"!\n", file_path );
        return 1;
    }

    fseek( fp, 0, SEEK_END );
    len = ftell( fp );
    fseek( fp, 0, SEEK_SET );

    buf = malloc( len );
    if ( !buf )
    {
        printf( "Failed to allocate file!\n" );
        return 1;
    }

    fread( buf, 1, len, fp );
    fclose( fp );

    svm_cpu_t cpu;
    if ( !svm_init_cpu( &cpu, buf ) )
        return 1;

    free( buf );
    return 0;
}

bool svm_init_cpu( svm_cpu_t *cpu, void *file )
{
    svm_executable_t *exe = file;

    if ( exe->magic != 'EmvS' )
    {
        printf( "Corrupted file!\n" );
        return false;
    }

    uintptr_t base = ( uintptr_t )exe;
    uint8_t *instr_ptr = ( uint8_t * )( base + exe->entry_rva );
    size_t instr_size = 0;

    while ( 1 )
    {
        const cpu_result_t result = cpu_exec_instr( &instr_ptr, &instr_size );

        if ( result == CPU_ADVANCE )
        {
            instr_ptr += instr_size;

            continue;
        }

        if ( result == CPU_EXIT )
        {
            printf( "Program finished execution!\n" );

            break;
        }
    }

    return true;
}