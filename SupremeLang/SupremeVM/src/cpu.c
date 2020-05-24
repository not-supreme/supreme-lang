#include "../inc/shared.h"
#include "../inc/instr.h"
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
	case SVM_OPCODE_NOP:
		SVM_DEBUGLOG( "NOP" );
		return CPU_ADVANCE;
	case SVM_OPCODE_BREAK:
		SVM_DEBUGLOG( "BREAK" );
		return CPU_EXIT;
	case SVM_OPCODE_JMP:
	{
		SVM_DEBUGLOG( "JMP" );

		SVM_ASSERT( opcode->has_extended_info );
		SVM_ASSERT( !opcode->has_extended_opcode );

		svm_opcode_ext_info_t *ext_info = ( svm_opcode_ext_info_t * )opcode + 1;
		SVM_ASSERT( ext_info->operand_count == 1 );

		uint8_t *new_ip = NULL;

		if ( ext_info->operand_mode & 1 )
		{
			SVM_DEBUGLOG( "Operand is a memory address/offset!" );

			if ( ext_info->absolute_addressing )
			{
				SVM_DEBUGLOG( "Absolute addressing is used!" );

				new_ip = *instr + sizeof( svm_opcode_t ) + sizeof( svm_opcode_ext_info_t );
				*instr = new_ip;
			} else
			{
				SVM_DEBUGLOG( "Relative addressing is used!" );

				uint32_t *disp = *instr + sizeof( svm_opcode_t ) + sizeof( svm_opcode_ext_info_t );
				new_ip = *instr + *disp;
				*instr = new_ip;
			}
		} else
		{
			SVM_DEBUGLOG( "WIP!" );
		}

		SVM_DEBUGLOG( "new_ip=%p", new_ip );
	} return CPU_DONT_ADVANCE;
	}
}