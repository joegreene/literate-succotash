literate-succotash
==================
> A little fun with C/C++ multithreading, NASM x86-64 assembly, and math.

Harmonic Sum:

![Harmonic Sum](https://en.wikipedia.org/api/rest_v1/media/math/render/svg/4e15948216b708718574eb69a79d049a3701ba92)

Inspiration & What This Project Does:
-------------------------------------
- This project is mostly a means to test processor performance, learn more about multithreading, and optimize speed of a program.
- This project computes the harmonic sum for positive integers and is meant to be able to compute extreme positive integers (e.g. in the billions) in a quick amount of time.
- This project (currently) has C/C++ as a driver and assembly code as a computer (for the harmonic sum)

DISCLAIMER:
-----------
- As this program takes advantage of ymm registers (which work on 4 floats at a time only), this program only supports inputs that are multiples of four. To remedy this, I plan to implement a __leftover sum__ solution, which works as follows:
  1. Run `computehsum` for the highest integer that is a multiple of 4 and less than input. Sum that gets computed goes into the variable `total_sum`.
  2. Add any remaining terms to `total_sum`.
    
  - Example: 97 terms
    - Assembly: `total_sum = 1/1 + 1/2 + ... + 1/96` for terms 1..96 (where 96 % 4 == 0 and 96 < 97)
    - C/C++ (after or even inside assembly run): `total_sum = total_sum + (1/97)`

How to Run:
-----------
- Linux:
  - Remove the beginning underscore on the entry point lines in `computehsum.asm`, like so:
  
        `global _computehsum` to `global computehsum`
    	
        `_computehsum` to `computehsum`
    
    - NOTE: You may have to change the following lines as well (this was a personal fix to be able to assemble `computehsum.asm` on Mac OS X, untested on Linux):
    
    ```
    mov rax, numerators
    vmovupd    ymm15, [rax]
    ;...other code...
    mov        rax, adddenominatorsby
    vaddpd     ymm14, [rax]
    ```
    
    to the following:
    
    ```
    vmovupd    ymm15, [numerators]
    ;...other code...
    vaddpd     ymm14, [adddenimonatorsby]
    ```

  - Remove the beginning underscore on the entry point in `asm_time.asm`, like so:

    `global _gettime` to `global gettime`

    `_gettime` to `gettime`

  - Run `./build_work.sh && ./clean_dir.sh`
- Mac OS X:
  - If you made edits to each `.asm` file (e.g. from running on Linux), make sure to revert the changes.
  - Run `./build_work.sh && ./clean_dir.sh`
- Windows:
  - TBD (no idea due to their funky calling conventions, etc.)

Directory Listing:
------------------
- `c`: Holds the c-style version of the code
- `cpp`: Holds the c++-style version of the code
- `Help`: Miscellaneous files used to come up with code product
- `Utlity`: Holds the `debug.inc` and `debug.asm` to check the stack in the `.asm` files.
  - Called via macros `showymmregisters #`, `showxmmregisters #`, etc.

TODO:
-----
- Finish performing leftover sum (in both drivers).
  - To squeeze a LOT of performance out of this, possibly implement this in `computehsum.asm`
- Finish cdriver so it uses pthreads (this requires changing `computehsum.asm` so it returns `void`)
- Update comments per file
- (Possibly) change the names of each file
- Look for redundancies/possible updates to each file
- Find out why the following warning outputs on a Mac OS X build (after running `build_work.sh`):
  ```
    clang: warning: -lcpp_driver.lis: 'linker' input unused
    ld: warning: PIE disabled. Absolute addressing (perhaps -mdynamic-no-pic) not allowed in code signed PIE, but used in _computehsum from computehsum.o. To fix this warning, don't compile with -mdynamic-no-pic or link with -Wl,-no_pie
  ```
