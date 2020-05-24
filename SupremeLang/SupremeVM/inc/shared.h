#ifndef _SHARED_H
#define _SHARED_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <memory.h>

#define SVM_ASSERT(expr) { if ( !( expr ) ) { printf( "[ERROR] assertion \"%s\" failed!", #expr ); exit( 1 ); } }
#define SVM_DEBUGLOG(msg, ...) printf( "[DEBUG] "msg, __VA_ARGS__ ); printf( "\n" );

#endif // _SHARED_H