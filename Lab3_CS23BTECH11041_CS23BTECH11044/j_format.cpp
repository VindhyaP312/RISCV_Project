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
string J_format(string instruction)    // jal x5, 108
{
    int s_index = 0;
    string ins, rd, imm, opcode = "1101111";
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
    imm = immediate_binary(imm, 21);      // Find imm value in binary format of size 21 bit
    // if imm provided is not a valid value, then the error msg it returned from immediate_binary is returned to main fn as hexcode
    if(imm.find("Error")!=string::npos){
        return imm;
    }
    //  removes the last bit to make into a 20 bit number for jump instruction
    imm.pop_back();
    // concatenates all binary strings and calling bin_to_hex fn
    return bin_to_hex(imm.substr(0,1) + imm.substr(10,10) + imm.substr(9,1) + imm.substr(1,8) + rd + opcode);
}