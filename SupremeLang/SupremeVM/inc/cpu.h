#ifndef _CPU_H
#define _CPU_H

typedef struct _svm_cpu_t
{
	uint16_t flags;
} svm_cpu_t;

typedef enum _cpu_result_t
{
	CPU_ADVANCE,
	CPU_DONT_ADVANCE,
	CPU_EXIT
} cpu_result_t;

cpu_result_t cpu_exec_instr( uint8_t **instr, size_t *out_size );

#endif // _CPU_H