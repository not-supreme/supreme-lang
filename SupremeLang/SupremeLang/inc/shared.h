#ifndef SL_SHARED_H
#define SL_SHARED_H

#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <llvm-c/Core.h>
#include <llvm-c/ExecutionEngine.h>
#include <llvm-c/Target.h>
#include <llvm-c/Transforms/Scalar.h>

#define SL_ASSERT( expr ) { if ( !( expr ) ) { printf( "[ERROR] assertion \"%s\" failed!", #expr ); exit( 1 ); } }
#define SL_ASSERT_LOG( expr, msg, ... ) { if ( !( expr ) ) { printf( "[ERROR] " msg "\n", __VA_ARGS__ ); exit( 1 ); } }
#define SL_DEBUG_LOG( msg, ... ) printf( "[DEBUG] " msg "\n", __VA_ARGS__ );

#endif // SL_SHARED_H
