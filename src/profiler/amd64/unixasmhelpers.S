
.intel_syntax noprefix
#include "unixasmmacros.inc"

//typedef void EnterNaked3(
//         rcx = FunctionIDOrClientID functionIDOrClientID);

//.align   16

.section .text
.globl EnterNaked3
.type EnterNaked3, @function
EnterNaked3:
        ret