#pragma once

#pragma pack(push, 1)

typedef struct _svm_cpu_t
{
	uint8_t *rip;
} svm_cpu_t;

typedef struct _svm_section_t
{
	uintptr_t sec_rva;
	uintptr_t sec_size;
} svm_section_t;

typedef struct _svm_executable_t
{
	uint32_t magic; // should be 'EmvS' (SvmE), used to validate file
	uint32_t version; // compiler version
	uint32_t extra_data[ 16 ]; // for future ABI compatibility
	svm_section_t section_code; // (RX) readable executable memory
	svm_section_t section_data; // (RW) readable writeable memory
} svm_executable_t;

#pragma pack(pop)

int svm_run_from_file( char *file_path );
bool svm_init_cpu( svm_cpu_t *cpu, void *file );