#include <iostream>
#include <fstream>
#include <string>
#include <algorithm> // for reversing string during deci_to_bin conversion
#include <cmath>      // for 'pow' function
using namespace std;
#include "assembler.h"

/* takes the whole insruction string as argument and returns 8 bit hexcode or any error message
* does string concatenation of binary codes of registers, imm and opcode in correct order
* calls bin_to_hex fn, which converts binary code to hexcode
*/
string U_format(string instruction)    // lui x3, 0x100
{
    int s_index = 0;
    string ins, rd, imm, opcode = "0110111";
    s_index = instruction.find(" ");
    ins = instruction.substr(0, s_index); // stores the name of the instruction
    instruction.erase(0, s_index + 1);
    s_index = instruction.find(",");
    rd = regname_to_binary(instruction.substr(0, s_index)); // stores destination register in binary format
    // if any register name is not a valid register name, it returns the error message that regname_to_binary function gave
    if(rd.find("Error")!=string::npos){
        return rd;
    }
    instruction.erase(0, s_index + 2);
    s_index = instruction.find(" ");
    imm = instruction.substr(0, s_index); // stores immediate value in string
    if(imm.empty()==true){
        return "Error: Instruction Expects 2 Arguments";
    }
    if(imm[0]=='-'){    
        return "Error: Immediate Value cannot be Negative";
    }
    // Find imm value in binary format of size 21 bit
    imm = immediate_binary(imm, 21);      
    // if imm provided is not a valid value, then the error msg it returned from immediate_binary is returned to main fn as hexcode
    if(imm.find("Error")!=string::npos){
        if(imm.find("Error: Immediate")!=string::npos)
            return "Error: Immediate value does not fit in 20 bits ";
        else
            return imm ;
    }
    // since the imm size should be 20 bits, the msb which is zero(since imm takes +ve values only) is removed in next step.
    imm.erase(0,1);
    // concatenates all binary strings and calling bin_to_hex fn
    return bin_to_hex(imm + rd + opcode);
}