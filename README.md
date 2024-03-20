# SimpleLoader Project

## Summary

This project aims to implement a SimpleLoader for loading an ELF 32-bit executable in plain C without using any library APIs available for manipulating ELF files. The SimpleLoader compiles to a shared library (`lib_simpleloader.so`) that can be used to load and execute the executable using a helper program.

## Usage
1. Download the ```files.zip``` from the repo 
2. Unzip the zip folder
3. open that folder in a code editor (preferably VS CODE)
4. run the ```make``` command
# Note
Individual directories have also been uploaded you can download individual zip and see each directory.

## Directories and Files

- Each folder contains a Makefile to compile the C file present in that directory.
- There is a top-level Makefile that compiles all the C files by internally invoking `make` command in each of the sub-directories.
- The "test" folder contains the `fib.c` as the testcase to be run using SimpleLoader.
- The executable `launch` and the library `lib_simpleloader.so` are automatically saved inside the "bin" folder by the Make command. The executable `fib` remains inside the "test" sub-directory.
- The project must be compiled with `-m32` flag for compatibility with 32-bit systems.
- The testcase `fib.c` is compiled with flags `-m32`, `-no-pie`, and `-nostdlib`.

## SimpleLoader Implementation (`loader.c`)

The SimpleLoader implementation follows the steps described in the assignment specifications. It loads and runs an executable by:
- Using system calls like `open` and `read` to read the content of the binary into heap-allocated memory.
- Iterating through the Program Header (PHDR) table to find the segment containing the entry point method.
- Allocating memory using `mmap` function and copying segment content.
- Navigating to the entry point address and calling the `_start` method.

## Implementation of Launcher (`launch.c`)

The `launch.c` is responsible for:
- Asking the user to provide the ELF file as a command-line argument.
- Carrying out necessary checks on the input ELF file.
- Passing it to the loader for loading/execution and invoking the cleanup routine inside the loader.

## Requirements

- Strictly follow the provided instructions, including directory structure and error checking.
- Proper documentation should be done in the code.
- Submission includes a zip file containing source files and a design document detailing the implementation.
- Only one submission per group is allowed.

## Reference Materials
![image](https://github.com/Aahan22001IIITD/PROJECTS/assets/125281835/c72bdffe-6486-448e-a228-e3b3178e3dff)
- [ELF File Format Documentation](https://man7.org/linux/man-pages/man5/elf.5.html)

## NOTE

### Hardware requirements:
- You will require a machine having any Operating System that supports Unix API. You will not be able to do this assignment on MacOS. You can either have a dual boot system having any Linux OS (e.g., Ubuntu), or install a WSL.

### Software requirements:
- C compiler and GNU make.
