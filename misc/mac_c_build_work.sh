#!/bin/bash

# assemble time object code
nasm -f macho64 -Ox -l asm_time.lis -o asm_time.o asm_time.asm

# assemble computehsum object code
nasm -f macho64 -Ox -l computehsum.lis -o computehsum.o computehsum.asm

# compile cdriver
gcc -c -m64 -Wall -l cdriver.lis -o cdriver.o cdriver.c -pthread

# link object files together to make final executable (harmonicsum.out)
gcc -m64 -o harmonicsum.out cdriver.o computehsum.o asm_time.o

