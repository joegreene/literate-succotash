literate-succotash
====================
>A little fun with C++ multithreading and NASM x86-64 assembly.

How to Run:
-----------
- Linux:
  - Remove the beginning underscore on the entry point in `computehsum.asm`, like so:
  	
  	`global _computehsum` --> `global computehsum`

  	`_computehsum` --> `computehsum`

  - Remove the beginning underscore on the entry point in `asm_time.asm`, like so:

    `global _gettime` to `global gettime`

    `_gettime` to `gettime`

  - Run `./build_work && ./clean_dir`
- Mac OS X:
	- If you made edits to the entry points in each `.asm` file (e.g. from running on Linux), 
	  make sure to put the underscores back in front.
  - Run `./build_work && ./clean_dir`
- Windows:
  - TBD (no idea due to their funky calling conventions, etc.)

Directory Listing:
------------------
- `c`: Holds the c-style version of the code
- `cpp`: Holds the c++-style version of the code
- `Help`: Miscellaneous files used to come up with code product
- `Utlity`: Holds the `debug.inc` and `debug.asm` to check the stack in the `.asm` files.
  - Called via entry points such as `showymmregisters #`, `showxmmregisters #`, etc.

TODO:
-----
- Finish cdriver so it uses pthreads (this requires changing `computehsum.asm` so it returns `void`)
- Update comments per file
- (Possibly) change the names of each file
- Look for redundancies/possible updates to each file
- Find out why the following warning outputs on a Mac OS X build (after running `build_work.sh`):
  ```
    clang: warning: -lcpp_driver.lis: 'linker' input unused
    ld: warning: PIE disabled. Absolute addressing (perhaps -mdynamic-no-pic) not allowed in code signed PIE, but used in _computehsum from computehsum.o. To fix this warning, don't compile with -mdynamic-no-pic or link with -Wl,-no_pie
  ```
