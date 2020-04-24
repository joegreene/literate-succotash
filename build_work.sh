#!/bin/bash

# assemble time object code
nasm -f elf64 -Ox -l asm_time.lis -o asm_time.o asm_time.asm

# assemble computehsum object code
nasm -f elf64 -Ox -l computehsum.lis -o computehsum.o computehsum.asm

# compile c_driver
g++ -c -m64 -Wall -l cpp_driver.lis -o cpp_driver.o cppdriver.cpp -std=c++11

# link object files together to make final executable (hsum.out)
g++ -m64 -std=c++11 -pthread -no-pie -o hsum.out cpp_driver.o computehsum.o asm_time.o
