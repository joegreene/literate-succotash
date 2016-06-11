;Author information
;  Author name: Joseph Greene
;  Author email: joeisgreene@csu.fullerton.edu
;  Author location: CSUF
;Project information
;  Project title: Harmonic Series Calculator
;  Purpose: To compute the harmonic sum of an integer divisible by four. Mainly, this program is a means 
;           to test processor performance and optimize speed of the program.
;  Status: Complete
;  Project files: cdriver.cpp, computehsum.asm, time.asm, macros.inc
;Module information
;  File name: asmdriver.asm
;  This module's call name: computehsum (_computehsum if on Mac OS X)
;  Language: X86-64
;  Syntax: Intel
;  Date last modified: 2016-06-02
;  Purpose:  To compute the harmonic sum of a range of integers (that is divisible by 4).
;  Status: Complete
;  Constraints: None
;  Future enhancements: None so far.
;Translator information
;  Linux:  nasm -f elf64 -Ox -l asm_computehsum.lis -o computehsum.o assign6-computehsum.asm
;  MacOSX: nasm -f macho64 -Ox -l asm_computehsum.lis -o computehsum.o assign6-computehsum.asm
;References and credits
;  CS 240 Holliday website (for majority of the macro work)
;Format information
;  Page width: 172 columns
;  Begin comments: 61
;  Optimal print specification: Landscape, 7 points, monospace, 8½x11 paper
;
;========== Global declarations ===========================================================================================================================================

%include "general-macros.inc"                               ;This file contains various macros that will be used in this driver

global _computehsum                                         ;Make this program callable by other programs.

segment .data                                               ;Initialized data are declared here

numerators        dq 1.0, 1.0, 1.0, 1.0                     ;Numerator values for summation
adddenominatorsby dq 3.0, 2.0, 1.0, 0.0                     ;Add (to initial denominators) by this value each
increment         dq 4.0, 4.0, 4.0, 4.0                     ;Values to increment denominator by

segment .bss                                                ;Place uninitialized data here

align 16
;Nothing in here

;===== Begin executable instructions here =================================================================================================================================

segment .text                                               ;Place executable instructions in this segment.

_computehsum:                                               ;Entry point.  Execution begins here.

;================= Back up section ========================================================================================================================================
backupgpr                                                   ;Macro to back up general purpose registers

;==========================================================================================================================================================================
;===== Begin the application here: do harmonic sum stuff ==================================================================================================================
;==========================================================================================================================================================================

;======== Compute each term via a for loop (assume n > 0) =================================================================================================================

;Preconditions: rdi holds start, rsi holds last term to compute to, rdx holds address to store sum, rcx holds address to store last term
;Postconditions: xmm0 holds the sum, xmm11 holds the last term

;General format (initial values):
;  ymm15 = numerators    = [  1.0  |   1.0   |   1.0   |   1.0   ]
;  ymm14 = denominators  = [ START | START+1 | START+2 | START+3 ] (e.g. first run is 1.0, 2.0, 3.0, 4.0)
;  ymm13 = increments    = [  4.0  |   4.0   |   4.0   |   4.0   ]
;  ymm12 = sums          = [  0.0  |   0.0   |   0.0   |   0.0   ]
;  ymm11 = reciprocals   = [  0.0  |   0.0   |   0.0   |   0.0   ]
;  ...
;

mov rax, numerators                                         ;Bug in macho64 won't allow [numerators] dereference
vmovupd    ymm15, [rax]                                     ;Set up numerator values in ymm15 (1.0, 1.0, 1.0, 1.0)

push qword 0                                                ;Reserve a qword on the stack for distributing the denominator
cvtsi2sd   xmm14, rdi                                       ;Convert the start integer to a float
movsd      [rsp], xmm14                                     ;Move the converted start integer to the top of the stack
vbroadcastsd ymm14, [rsp]                                   ;Broadcast the integer-converted float throughout ymm14
pop        rax                                              ;Undo push instruction made four instructions previous

mov        rax, adddenominatorsby                           ;Bug in macho64 won't allow [adddenominatorsby] dereference directly to ymm14 (this is a workaround)
vaddpd     ymm14, [rax]                                     ;Add to denominators by 0.0, 1.0, 2.0, 3.0 to initialize initial denominators

mov        rax, increment                                   ;Bug in macho64 won't allow [increment] dereference directly to ymm13 (this is a workaround)
vmovupd    ymm13, [rax]                                     ;Set up increment value (4.0, 4.0, 4.0, 4.0)
vxorpd     ymm12, ymm12                                     ;Zero-out the accumulating register (initially all zero)
vxorpd     ymm11, ymm11                                     ;Zero-out the reciprocal destination (initially all zero)

domoreterms:                                                ;Label for term computation loop
  cmp      rdi, rsi                                         ;Compare highest denominator value with number of terms to compute 
  ja       stopcompute                                      ;Jump out if last term has been computed

  vmovupd  ymm11, ymm15                                     ;Copy numerator over for reciprocal calculation
  vdivpd   ymm11, ymm14                                     ;reciprocal (ymm11) = numerator (temporarily stored in ymm11 from ymm15) / denominator (ymm14)

  vaddpd   ymm12, ymm11                                     ;current_sum (ymm12) = old_sum (ymm12) + current terms (ymm11)
  vaddpd   ymm14, ymm13                                     ;Increment denominator by 4, i.e. ymm14 = ymm14 (denominators) + ymm13 (increment)
  
  add      rdi, 4                                           ;Add four to rdi (as an increment since we're doing 4 operations per iteration)

  jmp      domoreterms                                      ;Jump back up to top of loop to loop again

stopcompute:                                                ;Label for ending sum computation

;======== Set return sum (i.e. the sum of all the positions in ymm12) =====================================================================================================

vhaddpd    ymm0, ymm12, ymm12                               ;Assume ymm12 = [D|C|B|A]. Now ymm0 equals [D+C|D+C|B+A|B+A]
vextractf128 xmm2, ymm0, 1                                  ;Move high of ymm0 to xmm2 (i.e. low of ymm2)
addsd      xmm0, xmm2                                       ;Now xmm0 equals A+B+C+D

;======== Send value to double reference parameters =======================================================================================================================

;movsd      [rdx], xmm0                                      ;As rdx holds the address to the total_sum in the driver, dereference the address 
movsd      [rdx], xmm11                                     ;As rdx holds the address to the last_term in the driver, dereference the address


;==========================================================================================================================================================================
;===== Component Restore ==================================================================================================================================================
;==========================================================================================================================================================================

restoregpr                                                  ;Macro to restore general purpose registers

ret                                                         ;Return back to caller
