#include "../inc/shared.h"
#include "../inc/instr.h"
#include "../inc/svm.h"
#include "../inc/cpu.h"

cpu_result_t cpu_exec_instr( svm_state_t *vm_state, uint8_t **instr, size_t *out_size )
{
	size_t encoded_size = sizeof( svm_opcode_t ); 
	svm_opcode_t *opcode = ( svm_opcode_t *)( *instr );
	uint16_t operation = opcode->operation;

	if ( opcode->has_extended_opcode )
	{
		operation += ( *instr )[ 1 ];
		encoded_size++;
	}

	if ( opcode->has_extended_info )
	{
		encoded_size += 2;
	}

	*out_size = encoded_size;

	switch ( operation )
	{
	case SVM_OPCODE_HLT:
		SVM_DEBUGLOG( "HLT" );
		return CPU_EXIT;
	case SVM_OPCODE_NOP:
		SVM_DEBUGLOG( "NOP" );
		return CPU_ADVANCE;
	case SVM_OPCODE_JMP:
	{
		SVM_DEBUGLOG( "JMP" );

		SVM_ASSERT( opcode->has_extended_info );
		SVM_ASSERT( !opcode->has_extended_opcode );

		svm_opcode_ext_info_t *ext_info = ( svm_opcode_ext_info_t * )( opcode + 1 );
		SVM_ASSERT( ext_info->operand_count == 1 );

		uint8_t *new_ip = NULL;

		if ( ext_info->operand_mode & 1 )
		{
			SVM_DEBUGLOG( "Operand is a memory address/offset!" );

			if ( ext_info->absolute_addressing )
			{
				SVM_DEBUGLOG( "Absolute addressing is used!" );

				new_ip = *instr + sizeof( svm_opcode_t ) + sizeof( svm_opcode_ext_info_t );
			} else
			{
				SVM_DEBUGLOG( "Relative addressing is used!" );

				uint32_t *disp = ( uint32_t * )( *instr + sizeof( svm_opcode_t ) + sizeof( svm_opcode_ext_info_t ) );
				new_ip = *instr + *disp;
			}
		} else
		{
			SVM_DEBUGLOG( "Operand is a dereferencing a register!" );

			uint8_t *register_id = ( uint8_t * )( *instr + sizeof( svm_opcode_t ) + sizeof( svm_opcode_ext_info_t ) );
			SVM_ASSERT( *register_id < 16 );
			uint64_t code_rva = vm_state->registers[ *register_id ];
			new_ip = ( ( uintptr_t )vm_state->executable_header ) + vm_state->executable_header->section_code.sec_rva + code_rva;
		}

		SVM_DEBUGLOG( "new_ip=%p", new_ip );
		*instr = new_ip;
	} return CPU_DONT_ADVANCE;
	case SVM_NATIVE_FASTCALL:
	{
		SVM_DEBUGLOG( "NATIVE_FASTCALL" );
	} return CPU_ADVANCE;
	}
}