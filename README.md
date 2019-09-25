# literate-succotash

![alt text](https://en.wikipedia.org/api/rest_v1/media/math/render/svg/4e15948216b708718574eb69a79d049a3701ba92 "Harmonic Sum (image from Wikipedia)")

`literate-succotash` is an efficient harmonic sum calculator written in C/C++, targeting Intel x86-64 processors with the AVX2 instruction set included. It focuses specifically on computing large positive N-values (e.g. in the billions) of the harmonic sum in a quick amount of time.

## Purpose

Although this project initially started out as a homework assignment for an assembly class, it eventually evolved into an exercise to do the following:

- See really big positive integers
- Become better acquainted with multithreading (in C and C++)
- Improve my x86-64 assembly programming skills
- Understanding clock cycle counts (and ultimately optimizing to a crazy degree for fun)

### Build Requirements

- An x86-64 Intel CPU with AVX2 instructions
- `nasm`
- `gcc` (to run the c version)
- `g++` (to run the c++ version)

### Usage

This project is designed to work on Linux (so far only tested on Ubuntu 16.04), MacOSX, and Windows (only tested on Windows 10 so far).
- Linux:
  - Run `./build_work.sh && ./clean_dir.sh` in the respective `c` or `cpp` folders
- Mac OS X:
  - Run `./build_work.sh && ./clean_dir.sh` in the respective `c` or `cpp` folders
- Windows:
  - TBD (no idea due to their funky calling conventions, etc.)
  
Future plans include writing a make file or some equivalent structure to determine environment settings to make the build process simpler.

### Future Plans

- Update documentation
- Add tests
- Create a proper makefile (to do easy compilation on different operating systems)
- Refactor code
- Find out why the following warning outputs on a Mac OS X build (after running `build_work.sh`), pretty sure it's due to the workaround in `computehsum.asm`:
  ```
    clang: warning: -lcpp_driver.lis: 'linker' input unused
    ld: warning: PIE disabled. Absolute addressing (perhaps -mdynamic-no-pic) not allowed in code signed PIE, but used in _computehsum from computehsum.o. To fix this warning, don't compile with -mdynamic-no-pic or link with -Wl,-no_pie
  ```
