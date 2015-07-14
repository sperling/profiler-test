
.intel_syntax noprefix
#include "unixasmmacros.inc"

//typedef void EnterNaked3(
//         rcx = FunctionIDOrClientID functionIDOrClientID);

        .align   16

LEAF_ENTRY EnterNaked3, _TEXT

        // save registers
        push    rax
        alloc_stack 8

        push    r10
        alloc_stack 8

        push    r11
        alloc_stack 8

        //sub     rsp, 20h
        alloc_stack 20h

        END_PROLOGUE

        call    EXTERNAL_C_FUNC(EnterStub)

        //add     rsp, 20h
	free_stack 20h

        // restore registers
        pop     r11
        pop     r10
        pop     rax

        // return
        ret

LEAF_END EnterNaked3, _TEXT
