#include <iostream>
#include <fstream>
#include <string>
#include <algorithm> // for reversing string during deci_to_bin conversion
#include <cmath>      // for 'pow' function
using namespace std;
#include "assembler.h"

/* takes the whole insruction string as argument and returns 8 bit hexcode or any error message
* does string concatenation of binary codes of registers, funct3, imm and opcode in correct order
* calls bin_to_hex fn, which converts binary code to hexcode
*/
string B_format(string instruction)    // beq x3, x5, 30
{
    int s_index = 0;
    string ins, rs2, rs1, imm, funct3, opcode = "1100011";
    s_index = instruction.find(" ");
    ins = instruction.substr(0, s_index); // stores the name of the instruction
    instruction.erase(0, s_index + 1);
    s_index = instruction.find(",");
    rs1 = regname_to_binary(instruction.substr(0, s_index)); // stores second source register in binary format
    // if any register name is not a valid register name, it returns the error message that regname_to_binary function gave
    if(rs1.find("Error")!=string::npos){
        return rs1;
    }
    instruction.erase(0, s_index + 2);
    s_index = instruction.find(",");
    rs2 = regname_to_binary(instruction.substr(0, s_index)); // stores second source register in binary format
    if(rs2.find("Error")!=string::npos){
        return rs2;
    }
    instruction.erase(0, s_index + 2);
    s_index = instruction.find(",");
    imm = instruction.substr(0, s_index); // stores immediate value in string
    if(imm.empty()==true){
        return "Error: Instruction Expects 3 Arguments";
    }
    imm = immediate_binary(imm, 13);      // Find imm value in binary format of size 13 bit
    // if imm provided is not a valid value
    if(imm.find("Error")!=string::npos){
        return imm;
    }
    //  removes the last bit to make into a 12 bit number for branch instruction
    imm.pop_back();  
    // based on instruction name, it gets their funct3 value
    if (ins == "beq")
        funct3 = "000";
    else if (ins == "bne")
        funct3 = "001";
    else if (ins == "blt") 
        funct3 = "100";
    else if (ins == "bge")
        funct3 = "101";
    else if (ins == "bltu")
        funct3 = "110";
    else if (ins == "bgeu")
        funct3 = "111";
    // concatenates all binary strings and calling bin_to_hex fn
    return bin_to_hex(imm.substr(0,1) + imm.substr(2,6) + rs2 + rs1 + funct3 + imm.substr(8,4) + imm.substr(1,1) + opcode);
}