#ifndef _CPU_H
#define _CPU_H

enum
{
	REGISTER_0,
	REGISTER_1,
	REGISTER_2,
	REGISTER_3,
	REGISTER_4,
	REGISTER_5,
	REGISTER_6,
	REGISTER_7,
	REGISTER_8,
	REGISTER_9,
	REGISTER_A,
	REGISTER_B,
	REGISTER_C,
	REGISTER_D,
	REGISTER_E,
	REGISTER_F,
	REGISTER_MAX
};

typedef struct _svm_state_t
{
	uint16_t flags;

	union
	{
		uint64_t registers[ REGISTER_MAX ];
		uint64_t r0;
		uint64_t r1;
		uint64_t r2;
		uint64_t r3;
		uint64_t r4;
		uint64_t r5;
		uint64_t r6;
		uint64_t r7;
		uint64_t r8;
		uint64_t r9;
		uint64_t ra;
		uint64_t rb;
		uint64_t rc;
		uint64_t rd;
		uint64_t re;
		uint64_t rf;
	};

	svm_executable_hdr_t *executable_header;
} svm_state_t;

/* This is the result of each instruction. */
typedef enum _cpu_result_t
{
	/* Advance to the next instruction. */
	CPU_ADVANCE,
	/* Don't advance to the next instruction as the instruction pointer was
	   modified as a result of the recently executed instruction. */
	CPU_DONT_ADVANCE,
	/* Stop executing instructions. */
	CPU_EXIT
} cpu_result_t;

/* Execute an instruction. */
cpu_result_t cpu_exec_instr(
	/* Contains cpu state. */
	svm_state_t *vm_state,
	/* Pointer to pointer to instruction. */
	uint8_t **instr,
	/* Pointer to variable that recieves size of instruction executed,
	   required for advancing instruction (if necessary). */
	size_t *out_size );

#endif // _CPU_H