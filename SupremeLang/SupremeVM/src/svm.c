#include "../inc/shared.h"
#include "../inc/cpu.h"
#include "../inc/instr.h"
#include "../inc/svm.h"

int svm_run_test( )
{
    void *buf = malloc( sizeof( svm_executable_hdr_t ) + 0x1000 );

    SVM_ASSERT( buf != NULL );

    svm_executable_hdr_t *exe_hdr = ( svm_executable_hdr_t * )buf;
    svm_section_hdr_t *sec_hdr = ( svm_section_hdr_t * )&exe_hdr->section_code;
    uint8_t *code_section = ( uint8_t * )( ( uintptr_t )buf + sizeof( svm_executable_hdr_t ) );

    memset( exe_hdr, 0, sizeof( svm_executable_hdr_t ) );
    memset( sec_hdr, 0, sizeof( svm_section_hdr_t ) );
    memset( code_section, 0, 0x1000 );

    sec_hdr->sec_rva = sizeof( svm_executable_hdr_t );
    sec_hdr->sec_size = 0x1000;

    exe_hdr->magic = 'EmvS';
    exe_hdr->version = 1;
    exe_hdr->entry_rva = 0;

    uint8_t assembled_code0[ ] =
    {
        SVM_OPCODE_NOP,
        SVM_OPCODE_NOP,
        SVM_OPCODE_NOP,
        SVM_OPCODE_NOP,
        0,
        0,
        0,
        0,
        0,
        0,
        0
    };

    svm_opcode_t *jmp_opcode = ( svm_opcode_t * )&assembled_code0[ 4 ];
    jmp_opcode->operation = SVM_OPCODE_JMP;
    jmp_opcode->has_extended_opcode = 0;
    jmp_opcode->has_extended_info = 1;

    svm_opcode_ext_info_t *jmp_xinfo = ( svm_opcode_ext_info_t * )&assembled_code0[ 5 ];
    jmp_xinfo->operand_count = 1;
    jmp_xinfo->operand_mode = 0b0001;
    jmp_xinfo->absolute_addressing = 0;

    int32_t *disp32 = &assembled_code0[ 7 ];
    *disp32 = 508; // 512 - the four bytes already executed

    uint8_t assembled_code1[ ] =
    {
        SVM_OPCODE_NOP,
        SVM_OPCODE_NOP,
        SVM_OPCODE_NOP,
        SVM_OPCODE_NOP,
        SVM_OPCODE_HLT
    };

    memcpy( code_section, &assembled_code0[ 0 ], sizeof( assembled_code0 ) );
    memcpy( code_section + 512, &assembled_code1[ 0 ], sizeof( assembled_code1 ) );

    svm_state_t vm_state;
    if ( !svm_init_cpu( &vm_state, buf ) )
    {
        free( buf );
        return 1;
    }

    free( buf );
    return 0;
}

int svm_run_from_file( char *file_path )
{
    FILE *fp;           // File pointer
    long len;           // File length (in bytes)
    char *buf;          // Pointer to file buffer
    
    if ( fopen_s( &fp, file_path, "rb" ) != 0 )
    {
        printf( "Failed to open file \"%s\"!\n", file_path );
        return 1;
    }

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

    svm_state_t vm_state;
    if ( !svm_init_cpu( &vm_state, buf ) )
        return 1;

    free( buf );
    return 0;
}

bool svm_init_cpu( svm_state_t *vm_state, void *file )
{
    svm_executable_hdr_t *exe = file;

    if ( exe->magic != 'EmvS' )
    {
        printf( "Corrupted file!\n" );
        return false;
    }

    uintptr_t base = ( uintptr_t )exe;
    uint8_t *instr_ptr = ( uint8_t * )( base + exe->section_code.sec_rva + exe->entry_rva );
    size_t instr_size = 0;

    while ( 1 )
    {
        const cpu_result_t result = cpu_exec_instr( vm_state, &instr_ptr, &instr_size );

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