#include <iostream>
#include <fstream>
#include <string>
#include <algorithm> // for reversing string during deci_to_bin conversion
#include <cmath>      // for 'pow' function
using namespace std;
#include "assembler.h"

/*
* takes the whole insruction string as argument and returns 8 bit hexcode or any error message
* does string concatenation of binary codes of registers, funct3, funct7 and opcode
* calls bin_to_hex fn, which converts binary code to hexcode
*/
string R_format(string instruction)
{
    //  r-type has 3 arguments, so should have 2 commas
    if(std::count(instruction.begin(),instruction.end(), ',' )!=2){   
        return "Error: Instruction expects 3 arguments ";
    }
    //   returns error string if space is not present b/w comma and next argument
    if(std::count(instruction.begin(),instruction.end(), ' ' )!=3){
        return "Error: Incorrect Format ";
    }
    int s_index = 0;
    string ins = "", rd = "", rs1 = "", rs2 = "", opcode = "0110011", funct3 = "", funct7 = "0000000";
    s_index = instruction.find(" ");
    ins = instruction.substr(0, s_index); // stores the name of the instruction
    instruction.erase(0, s_index + 1);
    s_index = instruction.find(",");
    rd = regname_to_binary(instruction.substr(0, s_index)); // stores destination register in 5 bit binary format
    instruction.erase(0, s_index + 2);
    s_index = instruction.find(",");
    rs1 = regname_to_binary(instruction.substr(0, s_index)); // stores source register 1 in 5 bit binary format
    instruction.erase(0, s_index + 2);
    s_index = instruction.find(",");
    rs2 = regname_to_binary(instruction.substr(0, s_index)); // stores source register 2 in 5 bit binary format

    // if any register name is not a valid register name
    if(rd.find("Error")!=string::npos){
        return rd;
    }

    if(rs1.find("Error")!=string::npos){
        return rs1;
    }

    if(rs2.find("Error")!=string::npos){
        return rs2;
    }

    // based on instruction name, it gets their funct3 and funct7
    if (ins.compare("add") == 0)
    {
        funct3 = "000";
    }
    else if (ins.compare("sub") == 0)
    {
        funct3 = "000";
        funct7 = "0100000";
    }
    else if (ins.compare("xor") == 0)
    {
        funct3 = "100";
    }
    else if (ins.compare("or") == 0)
    {
        funct3 = "110";
    }
    else if (ins.compare("and") == 0)
    {
        funct3 = "111";
    }
    else if (ins.compare("sll") == 0)
    {
        funct3 = "001";
    }
    else if (ins.compare("srl") == 0)
    {
        funct3 = "101";
    }
    else if (ins.compare("sra") == 0)
    {
        funct3 = "101";
        funct7 = "0100000";
    }
    // concatenates all binary strings and calling bin_to_hex fn
    return bin_to_hex(funct7 + rs2 + rs1 + funct3 + rd + opcode);
}

