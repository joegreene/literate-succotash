# literate-succotash
## Contents
- [Intro](#intro)
- [Disclaimer](#disclaimer)
- [How to Run](#how-to-run)
- [Directory Listing](#directory-listing)
- [What Needs To Be Completed](#what-needs-to-be-completed)

### Intro

![alt text](https://en.wikipedia.org/api/rest_v1/media/math/render/svg/4e15948216b708718574eb69a79d049a3701ba92 "Harmonic Sum (image from Wikipedia)")

- This project computes the harmonic sum for positive integers and is meant to be able to compute extreme positive integers (e.g. in the billions) in a quick amount of time.
- This project (currently) has a C and C++ version and assembly code as a computer (for the harmonic sum)
- This project is mostly a means to test processor performance, learn more about multithreading, and optimize speed of a program.

### How to Run
- Linux (untested):
  - Run `./build_work.sh && ./clean_dir.sh` in the respective `c` or `cpp` folders
- Mac OS X:
  - Run `./build_work.sh && ./clean_dir.sh` in the respective `c` or `cpp` folders
- Windows:
  - TBD (no idea due to their funky calling conventions, etc.)

### Directory Listing
- `linux` and `mac os x` folders: Holds C and C++ versions of the code set up per OS
- Per OS folder:
  - `c`: Holds the c-style version of the code (uses pthread for multithreading)
  - `cpp`: Holds the c++-style version of the code (uses std::future and std::async for multithreading)
- `Help`: Miscellaneous documents used to come up with code product
- `Utlity`: Holds the `debug.inc` and `debug.asm` to check the stack in the `.asm` files.
  - Called via macros `showymmregisters #`, `showxmmregisters #`, etc. in `debug.inc`

### What Needs To Be Completed
- Update comments per file
- (Possibly) change the names of each file
- Create a proper makefile (to do easy compilation on different OS's)
- Look for redundancies/possible updates to each file
- Find out why the following warning outputs on a Mac OS X build (after running `build_work.sh`), pretty sure due to the workaround in `computehsum.asm`:
  ```
    clang: warning: -lcpp_driver.lis: 'linker' input unused
    ld: warning: PIE disabled. Absolute addressing (perhaps -mdynamic-no-pic) not allowed in code signed PIE, but used in _computehsum from computehsum.o. To fix this warning, don't compile with -mdynamic-no-pic or link with -Wl,-no_pie
  ```
