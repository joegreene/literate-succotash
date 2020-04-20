;Author information
;  Author name: Joseph Greene
;Project information
;  Project title: Harmonic Series Computer
;  Purpose: To compute the harmonic sum of an integer divisible by four. Mainly, this program is a means 
;           to test processor performance and optimize speed of the program.
;  Status: Complete
;  Project files: assign6-cdriver.cpp, assign5-computehsum.asm, assign6-time.asm, assign6-macros.inc
;Module information
;  File name: assign6-time.asm
;  This module's call name: gettime
;  Language: X86-64
;  Syntax: Intel
;  Date last modified: 2015-Apr-23
;  Purpose:  To get the current ticks of the computer.
;  Status: Complete
;  Constraints: None
;  Future enhancements: None so far.
;Translator information
;  Linux: nasm -f elf64 -Ox -l asm_time.lis -o asm_time.o assign6-time.asm
;References and credits
;  CS 240 Holliday website (for all the macro work)
;
;========== Global declarations ===========================================================================================================================================

global  gettime                                            ;Make this program callable by other programs.

segment .data                                               ;Initialized data are declared here

;Nothing here

segment .bss                                                ;Uninitialized data here

;===== Begin executable instructions here =================================================================================================================================

segment .text                                               ;Place executable instructions in this segment.

gettime:                                                   ;Entry point.  Execution begins here.

;==========================================================================================================================================================================
;===== Begin the application here: get time ===============================================================================================================================
;==========================================================================================================================================================================

push       rdx                                              ;Back up rdx prior to call

xor        rax, rax                                         ;Zero-out rax (precondition for cpu time call)
xor        rdx, rdx                                         ;Zero-out rdx (precondition for cpu time call)

cpuid                                                       ;Needed for some reason
rdtsc                                                       ;Get time (put into low of rax and high of rdx respectively)

shl        rdx, 32                                          ;Shift rdx left by 32 bits (to make room for other half of time, which is in rdx)
or         rax, rdx                                         ;rax now holds the time (leave in there so it doesn't get overridden by restoregpr)

pop        rdx                                              ;Restore rdx

mov        rdi, rax                                         ;Copy time to rdi (for returning)

ret                                                         ;Return back to caller

