#include <iostream>
#include <fstream>
#include <string>
#include <algorithm> // for reversing string during deci_to_bin conversion
#include <cmath>      // for 'pow' function
using namespace std;
#include "riscv_header.h"

/*
* takes the whole insruction string as argument and returns 8 bit hexcode or any error message
* does string concatenation of binary codes of registers, funct3, funct7 and opcode
* calls bin_to_hex fn, which converts binary code to hexcode
*/
string R_format(string instruction)
{
    vector<string> v = parse(instruction);
    if(v[0].find("Error")!=string::npos){
        return v[0];
    }
    string ins = v[1], rd = v[2], rs1 = v[3], rs2 = v[4], opcode = "0110011", funct3 = "", funct7 = "0000000";
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

/*
takes the parsed vector of instruction 
Does its specific function on two source registers and stores the resulatant long int value in dest reg
*/
int R_execute(vector<string> part){
    string ins = part[1], rd = part[2], rs1 = part[3], rs2 = part[4];
    int i;
    // if x0 is dest reg, it should not be overwritten
    if(rd == "00000"){
        return 1;
    }
    if(ins == "add")    reg_value[rd] = reg_value[rs1] + reg_value[rs2];
    else if(ins == "sub")    reg_value[rd] = reg_value[rs1] - reg_value[rs2];
    else if(ins == "xor")    reg_value[rd] = reg_value[rs1] ^ reg_value[rs2];
    else if(ins == "or")     reg_value[rd] = reg_value[rs1] | reg_value[rs2];
    else if(ins == "and")    reg_value[rd] = reg_value[rs1] & reg_value[rs2];
    else if(ins == "sll"){
        unsigned long int rs2_val = reg_value[rs2];
        rs2_val = long(rs2_val%64);
        rs2_val = (rs2_val + 64)%64;
        reg_value[rd] = reg_value[rs1] <<rs2_val;
    }
    else if(ins == "srl"){  
        unsigned long int rs2_val = reg_value[rs2];
        rs2_val = long(rs2_val%64);
        rs2_val = (rs2_val + 64)%64;
        unsigned long int srl_result = reg_value[rs1] >> rs2_val;
        unsigned long int shift_deci = (1ULL << (64 - rs2_val)) - 1;
        srl_result = srl_result & shift_deci;
        reg_value[rd] = srl_result;
    }
    else if(ins == "sra"){
        unsigned long int rs2_val = reg_value[rs2];
        rs2_val = long(rs2_val%64);
        rs2_val = (rs2_val + 64)%64;
        reg_value[rd] = reg_value[rs1] >>rs2_val;
    }

    return 1;
}