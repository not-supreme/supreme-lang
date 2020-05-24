#ifndef _INSTR_H
#define _INSTR_H

/* Tells the processor what operation to perform on each instruction. */
typedef union _svm_opcode_t
{
	/* The full opcode byte. */
	uint8_t full_byte;

	struct
	{
		/* The operation this instruction will perform. */
		uint8_t operation : 6;
		/* If 1, the next byte will be an extension to the opcode, giving us 14 bits in total. */
		uint8_t has_extended_opcode : 1;
		/* If 1, the next word will contain extended information, such as size, operands, and more. */
		uint8_t has_extended_info : 1;
	};
} svm_opcode_t;

typedef union _svm_opcode_ext_info_t
{
	/* The full extended info word. */
	uint16_t full_word;

	struct
	{
		/* How many operands are used in the instruction? */
		uint16_t operand_count : 2;
		/* Bitfield indicating operand mode (register or memory?) for each operand.
		   Each bit represents an operand, obviously. 0 means register, 1 means memory. */
		uint16_t operand_mode : 3;
		/* If this bit is set, all memory accesses will be done directly at the value in the instruction.
		   This means that the VM will expect 64-bit addresses. If this bit is clear, all memory accesses
		   will be relative to the instruction pointer (relative to the first byte in the instruction), and
		   the VM will expect 32-bit addresses. */
		uint16_t absolute_addressing : 1;
	};
} svm_opcode_ext_info_t;

typedef enum _svm_e_opcode_t
{
	/* Stop execution. Whether or not execution continues is up to the host (VM). */
	SVM_OPCODE_HLT,
	/* No operation. Advance to next instruction. */
	SVM_OPCODE_NOP,
	/* Jump to certain instruction. */
	SVM_OPCODE_JMP,
	/* Move operation. */
	SVM_OPCODE_MOV,
	/* Call native function. */
	SVM_OPCODE_FASTCALL
} svm_e_opcode_t;

#endif // _INSTR_H