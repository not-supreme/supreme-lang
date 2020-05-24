#ifndef _SVM_H
#define _SVM_H

#pragma pack(push, 1)

typedef struct _svm_section_t
{
	uint16_t sec_flags;
	uintptr_t sec_rva;
	uintptr_t sec_size;
} svm_section_t;

typedef struct _svm_executable_hdr_t
{
	uint32_t magic;             // should be 'EmvS' (SvmE), used to validate file
	uint32_t version;           // compiler version
	uintptr_t entry_rva;        // rva to entrypoint
	svm_section_t section_code; // code, executable
	svm_section_t section_data; // data, non-executable
	/* This is to make sure that the size of this structure is 4096 (0x1000) aka PAGE_SIZE.
	   This means that if we have code right after the executable header it will be page aligned. */
	uint8_t padding[ 0x1000 - sizeof( struct _svm_executable_hdr_t ) ];
} svm_executable_hdr_t;

#pragma pack(pop)

int svm_run_from_file( char *file_path );
bool svm_init_cpu( svm_state_t *cpu, void *file );

#endif // _SVM_H