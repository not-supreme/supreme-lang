#ifndef SUPREMELANG_BACKEND_AMD64_H
#define SUPREMELANG_BACKEND_AMD64_H

extern e_backend_error amd64_assemble32( char *code, uint8_t **encoded, size_t *size );
extern e_backend_error amd64_assemble64( char *code, uint8_t **encoded, size_t *size );

#endif // SUPREMELANG_BACKEND_AMD64_H
