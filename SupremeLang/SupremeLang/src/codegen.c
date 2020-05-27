#include "../inc/shared.h"
#include "../inc/result.h"
#include "../inc/token.h"
#include "../inc/ast.h"
#include "../inc/codegen.h"

typedef int( *sl_llvm_main_fn_t )( );

sl_result_t sl_codegen_init( sl_codegen_t *codegen, const char *const module_name )
{
	codegen->llvm_context = LLVMContextCreate( );
	codegen->llvm_module = LLVMModuleCreateWithNameInContext( module_name, codegen->llvm_context );
	codegen->llvm_builder = LLVMCreateBuilderInContext( codegen->llvm_context );
	codegen->llvm_pass_man = LLVMCreateFunctionPassManagerForModule( codegen->llvm_module );

	LLVMAddInstructionCombiningPass( codegen->llvm_pass_man );
	LLVMAddReassociatePass( codegen->llvm_pass_man );
	LLVMAddGVNPass( codegen->llvm_pass_man );
	LLVMAddCFGSimplificationPass( codegen->llvm_pass_man );
	LLVMInitializeFunctionPassManager( codegen->llvm_pass_man );

	return SL_RES_OK;
}

sl_result_t sl_codegen_free( sl_codegen_t *codegen )
{
	if ( codegen->llvm_builder != NULL )
	{
		LLVMDisposeBuilder( codegen->llvm_builder );

		codegen->llvm_builder = NULL;
	}

	if ( codegen->llvm_module != NULL )
	{
		LLVMDisposeModule( codegen->llvm_module );

		codegen->llvm_module = NULL;
	}

	if ( codegen->llvm_context != NULL )
	{
		LLVMContextDispose( codegen->llvm_context );

		codegen->llvm_context = NULL;
	}

	if ( codegen->llvm_pass_man != NULL )
	{
		LLVMDisposePassManager( codegen->llvm_pass_man );

		codegen->llvm_pass_man = NULL;
	}

	return SL_RES_OK;
}

LLVMValueRef sl_codegen_compile_constant_expr( sl_codegen_t *codegen, sl_parser_node_t *ast_node, sl_codegen_error_t *out_error )
{
	sl_token_t *constant = ast_node->as.constant;

	switch ( constant->token_type )
	{
	case TOKEN_NUMBER: return LLVMConstReal( LLVMDoubleType( ), constant->as.number );
	case TOKEN_INTEGER: return LLVMConstInt( LLVMInt64TypeInContext( codegen->llvm_context ), constant->as.integer, false );
	case TOKEN_STRING: return LLVMConstStringInContext( codegen->llvm_context, constant->as.string, ( uint32_t )strlen( constant->as.string ), false );
	case TOKEN_KEYWORD_TRUE: return LLVMConstInt( LLVMInt1TypeInContext( codegen->llvm_context ), 1, false );
	case TOKEN_KEYWORD_FALSE: return LLVMConstInt( LLVMInt1TypeInContext( codegen->llvm_context ), 0, false );
	}

	return NULL;
}

LLVMValueRef sl_codegen_compile_unary_expr( sl_codegen_t *codegen, sl_parser_node_t *ast_node, sl_codegen_error_t *out_error )
{
	LLVMValueRef value = sl_codegen_compile_ast_node( codegen, ast_node->as.unary_expr.operand, out_error );

	if ( value == NULL )
		return NULL;

	bool use_fp_instr = LLVMIsAConstantFP( value );

	sl_token_t *operator = ast_node->as.unary_expr.op_token;

	switch ( operator->token_type )
	{
	case TOKEN_BANG: return LLVMBuildNot( codegen->llvm_builder, value, "tmp_not" );
	case TOKEN_MINUS: return use_fp_instr
		? LLVMBuildFNeg( codegen->llvm_builder, value, "tmp_neg" )
		: LLVMBuildNeg( codegen->llvm_builder, value, "tmp_neg" );
	}

	return NULL;
}

LLVMValueRef sl_codegen_compile_binary_expr( sl_codegen_t *codegen, sl_parser_node_t *ast_node, sl_codegen_error_t *out_error )
{
	LLVMValueRef lhs_value = sl_codegen_compile_ast_node( codegen, ast_node->as.binary_expr.left, out_error );

	if ( lhs_value == NULL )
		return NULL;

	LLVMValueRef rhs_value = sl_codegen_compile_ast_node( codegen, ast_node->as.binary_expr.right, out_error );

	if ( rhs_value == NULL )
		return NULL;

	bool use_fp_instr = LLVMIsAConstantFP( lhs_value ) || LLVMIsAConstantFP( rhs_value );

	sl_token_t *operator = ast_node->as.binary_expr.op_token;

	switch ( operator->token_type )
	{
	case TOKEN_EQUAL_EQUAL: return use_fp_instr
		? LLVMBuildFCmp( codegen->llvm_builder, LLVMRealOEQ, lhs_value, rhs_value, "tmp_eq" )
		: LLVMBuildICmp( codegen->llvm_builder, LLVMIntEQ, lhs_value, rhs_value, "tmp_eq" );

	case TOKEN_BANG_EQUAL: return use_fp_instr
		? LLVMBuildFCmp( codegen->llvm_builder, LLVMRealONE, lhs_value, rhs_value, "tmp_ne" )
		: LLVMBuildICmp( codegen->llvm_builder, LLVMIntNE, lhs_value, rhs_value, "tmp_ne" );

	case TOKEN_GREATER: return use_fp_instr
		? LLVMBuildFCmp( codegen->llvm_builder, LLVMRealOGT, lhs_value, rhs_value, "tmp_gt" )
		: LLVMBuildICmp( codegen->llvm_builder, LLVMIntSGE, lhs_value, rhs_value, "tmp_gt" );

	case TOKEN_GREATER_EQUAL: return use_fp_instr
		? LLVMBuildFCmp( codegen->llvm_builder, LLVMRealOGE, lhs_value, rhs_value, "tmp_ge" )
		: LLVMBuildICmp( codegen->llvm_builder, LLVMIntSGE, lhs_value, rhs_value, "tmp_ge" );

	case TOKEN_LESS: return use_fp_instr
		? LLVMBuildFCmp( codegen->llvm_builder, LLVMRealOLT, lhs_value, rhs_value, "tmp_lt" )
		: LLVMBuildICmp( codegen->llvm_builder, LLVMIntSLT, lhs_value, rhs_value, "tmp_lt" );

	case TOKEN_LESS_EQUAL: return use_fp_instr
		? LLVMBuildFCmp( codegen->llvm_builder, LLVMRealOLE, lhs_value, rhs_value, "tmp_le" )
		: LLVMBuildICmp( codegen->llvm_builder, LLVMIntSLE, lhs_value, rhs_value, "tmp_le" );

	case TOKEN_PLUS: return use_fp_instr
		? LLVMBuildFAdd( codegen->llvm_builder, lhs_value, rhs_value, "tmp_add" )
		: LLVMBuildAdd( codegen->llvm_builder, lhs_value, rhs_value, "tmp_add" );

	case TOKEN_MINUS: return use_fp_instr
		? LLVMBuildFSub( codegen->llvm_builder, lhs_value, rhs_value, "tmp_sub" )
		: LLVMBuildSub( codegen->llvm_builder, lhs_value, rhs_value, "tmp_sub" );

	case TOKEN_STAR: return use_fp_instr
		? LLVMBuildFMul( codegen->llvm_builder, lhs_value, rhs_value, "tmp_mul" )
		: LLVMBuildMul( codegen->llvm_builder, lhs_value, rhs_value, "tmp_mul" );

	case TOKEN_SLASH: return use_fp_instr
		? LLVMBuildFDiv( codegen->llvm_builder, lhs_value, rhs_value, "tmp_div" )
		: LLVMBuildSDiv( codegen->llvm_builder, lhs_value, rhs_value, "tmp_div" );
	}

	return NULL;
}

LLVMValueRef sl_codegen_compile_ast_node( sl_codegen_t *codegen, sl_parser_node_t *ast_node, sl_codegen_error_t *out_error )
{
	switch ( ast_node->node_type )
	{
	case AST_VARIABLE: SL_ASSERT_LOG( true == false, "variables are not supported yet" );
	case AST_EXPR_FUNCTION_DEF: SL_ASSERT_LOG( true == false, "function definitions are not supported yet" );

	case AST_CONSTANT: return sl_codegen_compile_constant_expr( codegen, ast_node, out_error );
	case AST_GROUP_EXPR: return sl_codegen_compile_ast_node( codegen, ast_node->as.group_expression, out_error );
	case AST_UNARY_EXPR: return sl_codegen_compile_unary_expr( codegen, ast_node, out_error );
	case AST_BINARY_EXPR: return sl_codegen_compile_binary_expr( codegen, ast_node, out_error );
	case AST_EXPR_STATEMENT: return sl_codegen_compile_ast_node( codegen, ast_node->as.expr_statement, out_error );
	}

	return NULL;
}
 
sl_result_t sl_codegen_compile( sl_codegen_t *codegen, sl_parser_node_t *ast_nodes, int32_t ast_node_count, sl_codegen_error_t *out_error )
{
	SL_ASSERT( codegen->llvm_context != NULL );
	SL_ASSERT( codegen->llvm_module != NULL );
	SL_ASSERT( codegen->llvm_builder != NULL );

	// Iterate over the AST nodes.
	for ( int32_t i = 0; i < ast_node_count; i++ )
	{
		LLVMTypeRef main_function_type = LLVMFunctionType( LLVMInt32TypeInContext( codegen->llvm_context ), NULL, 0, false );
		LLVMValueRef main_function = LLVMAddFunction( codegen->llvm_module, "main", main_function_type );
		LLVMBasicBlockRef entry = LLVMAppendBasicBlockInContext( codegen->llvm_context, main_function, "entry" );

		LLVMPositionBuilderAtEnd( codegen->llvm_builder, entry );

		LLVMValueRef compile_result = sl_codegen_compile_ast_node( codegen, &ast_nodes[ i ], out_error );

		if ( compile_result == NULL )
			return SL_RES_CODEGEN_ERROR;

		LLVMBuildRet( codegen->llvm_builder, compile_result );
	}

	// Dump the module
	LLVMDumpModule( codegen->llvm_module );

	return SL_RES_OK;
}
