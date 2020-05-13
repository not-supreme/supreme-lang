#ifndef _BACKEND_AMD64_H
#define _BACKEND_AMD64_H

e_backend_error amd64_assemble32( char *code, uint8_t **encoded, size_t *size );
e_backend_error amd64_assemble64( char *code, uint8_t **encoded, size_t *size );

void amd64_free_output( uint8_t *encoded );

#endif // _BACKEND_AMD64_H