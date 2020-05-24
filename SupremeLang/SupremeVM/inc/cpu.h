#ifndef _CPU_H
#define _CPU_H

typedef struct _svm_cpu_t
{
	uint16_t flags;
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