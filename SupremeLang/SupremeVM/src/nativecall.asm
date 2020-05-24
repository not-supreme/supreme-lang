;
; Native call intrinsics
;

.CODE

;
; RCX = callee address
; RDX = amount of arguments used
; extern "C" void __fastcall __native_fastcall( void *callee, uint64_t arg_count, uint64_t rcx, uint64_t rdx, uint64_t r8, uint64_t r9 );
;

__native_fastcall PROC
	int 3
	cmp rdx, 1
	jge __single_registerstate
	call rcx
	ret
__single_registerstate:
	push rcx
	mov rcx, r8
	cmp rdx, 2
	jge __double_registerstate
	call qword ptr [rsp]
	pop rcx
	ret
__double_registerstate:
	push rdx
	mov rdx, r9
	cmp qword ptr [rsp], 3
	jge __triple_registerstate
	call qword ptr [rsp - 08h]
	pop rdx
	pop rcx
	ret
__triple_registerstate:
	mov r8, [rsp - 18h]
	cmp qword ptr [rsp], 4
	jge __quad_registerstate
	call qword ptr [rsp - 08h]
	pop rdx
	pop rcx
	ret
__quad_registerstate:
	mov r9, [rsp - 20h]
	call qword ptr [rsp - 08h]
	pop rdx
	pop rcx
	ret
__native_fastcall ENDP

END