# Simulator README

### Nishi Baranwal, CS23BTECH11041  
### Paidala Vindhya, CS23BTECH11044

## Contents
1. [Download](#download)
2. [How To Use](#how-to-use)
3. [Cleaning](#cleaning)

## 1. Download

- First, download the zip file `Lab4_CS23BTECH11041_CS23BTECH11044.zip` and extract it on your computer.
- The folder will contain:
  - Header file - `riscv_header.h`
  - Source files - `main.cpp`, `assembler_gen_func.cpp`, `simulator_gen_func.cpp`, `b_format.cpp`, `i_format.cpp`, `j_format.cpp`, `r_format.cpp`, `s_format.cpp`, `u_format.cpp`, `hexcode_generator.cpp`
  - Input file - `input.s`: Write your assembly code in this file. If you have to run another file, please copy it into the `Lab4_CS23BTECH11041_CS23BTECH11044` folder.
  - Makefile
  - Test Cases, Test Cases 2 - Files containing cases we tested our code with while developing. Test Cases correspond to the testing of the assembler part, while Test Cases 2 correspond to the testing of the simulator part.
  - readme file
  - report file

- Open the folder in terminal and run the command:
```
make
```


This command will create the `.o` object files and the `riscv_sim` executable.

- You can also run the command `make all` instead of `make` to do the same thing.

## 2. How To Use

- Since we have already created the executable file: `riscv_sim` (by using the `make` command—please see the [Download](#download) section).

- To use the executable, the syntax is:
```
./riscv_sim
```

- To run the assembly code in a file (let’s say the file name is `input.s`):
- Run `./riscv_sim` on terminal. Now, use the following commands to perform their respective functions:
  - `load <file name>`:
    - Make sure the file being loaded is present in the folder. We have provided a file `input.s`, assembly code can be written in that file and run as well.
    - This command will load the input file containing RISC-V assembly code. All registers will get initialized to the default value. If any data is present in the `.data` section, it will get loaded in the `.data` section of memory. If no errors in the input file, the hex code of the instructions will be loaded in the `.text` section of memory.
    - If there are any errors in the input file, they will be printed. The code will not run unless the corrected file is loaded again.
    - There should be a single space between `load` and `<file name>`.
  - `run`:
    - This command will execute the given RISC-V code and update registers, memory, etc. It runs the given RISC-V code till the end.
  - `regs`:
    - This command will print the values of all registers in hex format (64-bit registers).
  - `exit`:
    - This command will exit the simulator.
  - `mem <addr> <count>`:
    - This command will print `count` memory locations starting from address `addr` (the memory is stored in Little Endian format).
    - `count` should be a non-negative value. If a negative value is provided, the error message "Invalid count value. Count value should be non-negative" is printed.
  - `step`:
    - This command will run one instruction and print the message "Executed `<instruction>`; PC=`<address>`".
  - `break <line>`:
    - This command sets a breakpoint to stop code execution once the line is reached, preserving the state of registers and memory.
    - Ensure that `<line>` is a valid line number. Otherwise, the error message "Breakpoint cannot be set. Line exceeds the last line." is printed.
  - `del break <line>`:
    - This command deletes the breakpoint at the specified line. If no breakpoint is present, the message "Breakpoint at `<line>` is not set" will be printed.
  - `show-stack`:
    - This command prints the stack information, and the elements pushed onto and popped from the stack frame. The stack frame is only updated on function invocations.

- The input file (containing the assembly code) should follow these formats:
- The input instructions should follow the format:
  ```
  instruction_name appropriate_operands
  ```
  where there is only 1 space between the instruction and the first operand. There should also be only 1 space between `,` and the second operand, and so on. Similarly, one colon after the label and then one space.
- Example:
  ```
  addi x1, x0, 4
  ```
- The program should start from the first character in each line.
- There should not be any blank lines in the input assembly code file.
- There should only be one instruction per line.
- There should not be any space after the instruction (even on the same line).
- There should not be any comments in the input assembly code file.
- There should not be any pseudo instructions in the input assembly code file.
- Label names cannot have spaces or colons.
- For the `.data` section, if it is present, then `.text` must also be present in the input file.
- In the `.data` section, for each type of data (`.dword`, `.word`, `.half`, `.byte`), if multiple values are present on the same line, they should be separated by a comma and a single space.
- There should not be any space after the (last) data value (even on the same line).
- The output for each line will be printed in the `output.hex` file.
- In the event the format is incorrect, the error will be printed in the `terminal` and in the `output.hex` file.

## 3. Cleaning

- To remove all the object files and the executable file created by the `make` command, run the following command:
```
make clean
```
- After running the above command your directory will be left with just the files that you
had initially downloaded, the input files (if added any) and the output file - output.hex .