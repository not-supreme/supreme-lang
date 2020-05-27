#ifndef SL_CODEGEN_H
#define SL_CODEGEN_H

typedef const char *sl_codegen_error_t;

typedef struct _sl_codegen_t
{
	LLVMContextRef llvm_context;
	LLVMModuleRef llvm_module;
	LLVMBuilderRef llvm_builder;
	LLVMPassManagerRef llvm_pass_man;
} sl_codegen_t;

/* Initializes the codegen structure.
   Always returns SL_RES_OK. */
sl_result_t sl_codegen_init( sl_codegen_t *codegen, const char *const module_name );

/* Frees the codegen structure.
   Always returns SL_RES_OK. */
sl_result_t sl_codegen_free( sl_codegen_t *codegen );

/* Compiles specific kinds of AST nodes to LLVM values.
   Returns a pointer to the actual LLVM value, or NULL if fails. */
LLVMValueRef sl_codegen_compile_constant_expr( sl_codegen_t *codegen, sl_parser_node_t *ast_node, sl_codegen_error_t *out_error );
LLVMValueRef sl_codegen_compile_unary_expr( sl_codegen_t *codegen, sl_parser_node_t *ast_node, sl_codegen_error_t *out_error );
LLVMValueRef sl_codegen_compile_binary_expr( sl_codegen_t *codegen, sl_parser_node_t *ast_node, sl_codegen_error_t *out_error );
LLVMValueRef sl_codegen_compile_ast_node( sl_codegen_t *codegen, sl_parser_node_t *ast_node, sl_codegen_error_t *out_error );

/* Compiles the given AST nodes. */
sl_result_t sl_codegen_compile( sl_codegen_t *codegen, sl_parser_node_t *ast_nodes,
	int32_t ast_node_count, sl_codegen_error_t *out_error );

#endif // SL_CODEGEN_H
