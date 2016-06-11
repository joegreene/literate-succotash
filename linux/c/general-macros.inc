;Author information
;  Author name: Joseph Greene
;  Author email: joeisgreene@csu.fullerton.edu
;  Author location: CSUF
;Course information
;  Course number: CPSC 240
;  Assignment number: 6
;  Due date: 2015-Apr-23
;Project information
;  Project title: Harmonic Series Computer
;  Purpose: To compute the harmonic sum of an integer divisible by four. Mainly, this program is a means 
;           to test processor performance and optimize speed of the program.
;  Status: Complete
;  Project files: assign6-cdriver.cpp, assign5-computehsum.asm, assign6-time.asm, assign6-macros.inc
;Module information
;  File name: assign6-macros.inc
;  This module's call name: does not apply.
;  Language: X86-64
;  Syntax: Intel
;  Date last modified: 2015-Apr-23
;  Purpose: This file hosts all the macros users in the assembly driver program
;  Status: Works under required specifications
;  Constraints: None
;  Future enhancements: None so far.
;Translator information
;  Does not assemble separately.  This module is assembled when the parent module is assembled.
;References and credits
;  CS 240 Holliday website (for all the macro work)
;Format information
;  Page width: 172 columns
;  Begin comments: 61
;  Optimal print specification: Landscape, 7 points, monospace, 8½x11 paper
;
;========== Global declarations ===========================================================================================================================================

segment .data                                               ;Initialized data are declared here

segment .bss                                                ;Uninitialized data here

align 64                                                    ;Ensure that next data declaration starts on a 64-byte boundary

savespace     resb 832                                      ;Storage used for xsave and xrstor during middle of program

;==========================================================================================================================================================================
;========== macro #1: backup GPR ==========================================================================================================================================
;==========================================================================================================================================================================

;Preconditions: None
;Postconditions: None

%macro backupgpr 0                                          ;Zero indicated that no data will be passed to this macro when it is called.
  push     rbp                                              ;Save a copy of the stack base pointer
  mov      rbp, rsp                                         ;This will preserve the linked list of base pointers.
  push     rbx                                              ;Back up rbx
  push     rcx                                              ;Back up rcx
  push     rdx                                              ;Back up rdx
  push     rsi                                              ;Back up rsi
  push     rdi                                              ;Back up rdi
  push     r8                                               ;Back up r8
  push     r9                                               ;Back up r9
  push     r10                                              ;Back up r10
  push     r11                                              ;Back up r11
  push     r12                                              ;Back up r12
  push     r13                                              ;Back up r13
  push     r14                                              ;Back up r14
  push     r15                                              ;Back up r15
  pushf                                                     ;Back up rflags

%endmacro

;==========================================================================================================================================================================
;========== macro #2: restore GPR =========================================================================================================================================
;==========================================================================================================================================================================

;Preconditions: None
;Postconditions: None

%macro restoregpr 0                                         ;Zero indicated that no data will be passed to this macro when it is called.
  popf                                                      ;Restore rflags
  pop      r15                                              ;Restore r15
  pop      r14                                              ;Restore r14
  pop      r13                                              ;Restore r13
  pop      r12                                              ;Restore r12
  pop      r11                                              ;Restore r11
  pop      r10                                              ;Restore r10
  pop      r9                                               ;Restore r9
  pop      r8                                               ;Restore r8
  pop      rdi                                              ;Restore rdi
  pop      rsi                                              ;Restore rsi
  pop      rdx                                              ;Restore rdx
  pop      rcx                                              ;Restore rcx
  pop      rbx                                              ;Restore rbx
  pop      rbp                                              ;Restore rbp

%endmacro

;========== End of assign6macros.inc ======================================================================================================================================

