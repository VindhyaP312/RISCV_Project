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
string S_format(string instruction)
{
    int s_index = 0;
    string ins, rs2, rs1, imm, funct3, opcode = "0100011";
    //  s-type has 3 arguments in this format: sd rs2, (imm)rs1 : so should have 1 comma
    if(std::count(instruction.begin(),instruction.end(), ',' )!=1){
        return "Error: Instruction expects 3 arguments ";
    }
    //   returns error string if space is not present b/w comma and next argument
    if(std::count(instruction.begin(),instruction.end(), ' ' )!=2){
        return "Error: Incorrect Format ";
    }
    s_index = instruction.find(" ");
    ins = instruction.substr(0, s_index); // stores the name of the instruction
    instruction.erase(0, s_index + 1);
    s_index = instruction.find(",");
    rs2 = regname_to_binary(instruction.substr(0, s_index)); // stores second source register in 5-bit binary format
    // if any register name is not a valid register name, it returns the error message that regname_to_binary function gave
    if(rs2.find("Error")!=string::npos){ 
        return rs2;
    }
    instruction.erase(0, s_index + 2);
    s_index = instruction.find("(");
    if(s_index==string::npos){
        return "Error: Invalid Format";
    }
    imm = instruction.substr(0, s_index); // stores immediate value in string
    if(imm.empty()==true){
        return "Error: Instruction Expects 3 Arguments";
    }
    instruction.erase(0, s_index + 1);
    s_index = instruction.find(")");
    if(s_index==string::npos){
        return "Error: Invalid Format";
    }
    rs1 = regname_to_binary(instruction.substr(0, s_index)); // stores first source register in 5-bit binary format
    if(rs1.find("Error")!=string::npos){
        return rs1;
    }
    imm = immediate_binary(imm, 12);      // Find imm value in binary format by calling immediate_binary fun and passing size as 12 bit
    // if imm provided is not a valid value
    if(imm.find("Error")!=string::npos){
        return imm;
    }
    // based on instruction name, it gets their funct3 value
    if (ins == "sb")
        funct3 = "000";
    else if (ins == "sh")
        funct3 = "001";
    else if (ins == "sw") 
        funct3 = "010";
    else if (ins == "sd")
        funct3 = "011";
    // concatenates all binary strings and calling bin_to_hex fn
    return bin_to_hex(imm.substr(0,7) + rs2 + rs1 + funct3 + imm.substr(7,5) + opcode);
}