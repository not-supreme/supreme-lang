#pragma once

typedef enum _e_backend_error
{
	BACKEND_SUCCESS,
	BACKEND_UNKNOWN_ERROR,
	BACKEND_AMD64_FAILED_KS_OPEN,
	BACKEND_AMD64_FAILED_KS_ASM
} e_backend_error;