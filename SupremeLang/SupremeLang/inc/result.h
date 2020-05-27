#ifndef SL_RESULT_H
#define SL_RESULT_H

typedef enum _sl_e_result_t
{
	SL_RES_OK,
	SL_RES_MEMORY_ALLOC_FAIL,
	SL_RES_MEMORY_FREE_FAIL,
	SL_RES_LEX_ERROR,
	SL_RES_PARSE_ERROR,
	SL_RES_CODEGEN_ERROR
} sl_e_result_t;

typedef int32_t sl_result_t;

#endif // SL_RESULT_H
