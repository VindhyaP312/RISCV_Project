#include <iostream>
#include <fstream>
#include <string>
#include <algorithm> // for reversing string during deci_to_bin conversion
#include <cmath>      // for 'pow' function
#include "assembler.h"
using namespace std;

string I_format(string instruction)
{
    int s_index = 0;
    string ins, rd, rs1, imm, funct3, funct6 = "000000", opcode;
    s_index = instruction.find(" ");
    ins = instruction.substr(0, s_index); // stores the name of the instruction
    //  checks for correct number of arguments and valid format o/w stores that error msg in hexcode and gets returned to main fn
    if (ins == "addi" || ins == "xori" || ins == "ori" || ins == "andi"){
        if(std::count(instruction.begin(),instruction.end(), ',' )!=2){
            return "Error: Instruction expects 3 arguments ";
        }
        if(std::count(instruction.begin(),instruction.end(), ' ' )!=3){
            return "Error: Incorrect Format ";
        }
    }
    else if (ins == "slli" || ins == "srli" || ins == "srai"){
        if(std::count(instruction.begin(),instruction.end(), ',' )!=2){
            return "Error: Instruction expects 3 arguments ";
        }
        if(std::count(instruction.begin(),instruction.end(), ' ' )!=3){
            return "Error: Incorrect Format ";
        }
    }
    else if (ins == "lb" || ins == "lh" || ins == "lw" || ins == "ld" || ins == "lbu" || ins == "lhu" || ins == "lwu"){
        if(std::count(instruction.begin(),instruction.end(), ',' )!=1){
            return "Error: Instruction expects 3 arguments ";
        }
        if(std::count(instruction.begin(),instruction.end(), ' ' )!=2){
            return "Error: Incorrect Format ";
        }
    }
    else if (ins == "jalr"){
        if(std::count(instruction.begin(),instruction.end(), ',' )!=1){
            return "Error: Instruction expects 3 arguments ";
        }
        if(std::count(instruction.begin(),instruction.end(), ' ' )!=2){
            return "Error: Incorrect Format ";
        }
    }
    instruction.erase(0, s_index + 1);
    s_index = instruction.find(",");
    rd = regname_to_binary(instruction.substr(0, s_index)); // stores destination register in binary format
    if(rd.find("Error")!=string::npos){
        return rd;
    }
    instruction.erase(0, s_index + 2);

    if (ins == "addi" || ins == "xori" || ins == "ori" || ins == "andi")
    {
        opcode = "0010011";
        s_index = instruction.find(",");
        rs1 = regname_to_binary(instruction.substr(0, s_index)); // stores source register in binary form
        if(rs1.find("Error")!=string::npos){
            return rs1;
        }
        instruction.erase(0, s_index + 2);
        s_index = instruction.find(",");
        imm = instruction.substr(0, s_index); // stores immediate value in string
        if(imm.empty()==true){
            return "Error: Instruction Expects 3 Arguments";
        }
        imm = immediate_binary(imm, 12);      // Find imm value in binary format by calling immediate_binary fun and passing size as 12 bit
        if(imm.find("Error")!=string::npos){
            return imm;
        }

        if (ins == "addi")
            funct3 = "000";
        else if (ins == "xori")
            funct3 = "100";
        else if (ins == "ori")
            funct3 = "110";
        else if (ins == "andi")
            funct3 = "111";

        return bin_to_hex(imm + rs1 + funct3 + rd + opcode);
    }
    // for shift instructions
    else if (ins == "slli" || ins == "srli" || ins == "srai")
    {
        opcode = "0010011";
        s_index = instruction.find(",");
        rs1 = regname_to_binary(instruction.substr(0, s_index)); // stores source register in binary form
        if(rs1.find("Error")!=string::npos){
            return rs1;
        }
        instruction.erase(0, s_index + 2);
        s_index = instruction.find(" ");
        imm = instruction.substr(0, s_index); // stores immediate value in string
        if(imm.empty()==true){
            return "Error: Instruction Expects 3 Arguments";
        }
        if(imm[0]=='-'){     // these instrs dont take negative offsets
            return "Error: Immediate Value cannot be Negative";
        }
        imm = immediate_binary(imm, 7);      // Find imm value in binary format by calling immediate_binary fun and passing size as 6 bit
        if(imm.find("Error")!=string::npos){
            if(imm.find("Error: Immediate")!=string::npos)
                return "Error: Immediate value does not fit in 6 bits ";
            else
                return imm ;
        }
        // removes the msb bit which was zero because it takes only positive offset
        imm.erase(0,1);

        if (ins == "slli")
        {
            funct3 = "001";
        }
        else if (ins == "srli")
        {
            funct3 = "101";
        }
        else if (ins == "srai")
        {
            funct3 = "101";
            funct6 = "010000";
        }

        return bin_to_hex(funct6 + imm + rs1 + funct3 + rd + opcode);
    }
    // for load instructions
    else if (ins == "lb" || ins == "lh" || ins == "lw" || ins == "ld" || ins == "lbu" || ins == "lhu" || ins == "lwu")
    {
        opcode = "0000011";
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
        rs1 = regname_to_binary(instruction.substr(0, s_index)); // stores source register in binary form
        if(rs1.find("Error")!=string::npos){
            return rs1;
        }
        imm = immediate_binary(imm, 12);      // Find imm value in binary format by calling immediate_binary fun and passing size as 12 bit
        if(imm.find("Error")!=string::npos){
            return imm;
        }
        if (ins == "lb")
            funct3 = "000";
        else if (ins == "lh")
            funct3 = "001";
        else if (ins == "lw") 
            funct3 = "010";
        else if (ins == "ld")
            funct3 = "011";
        else if (ins == "lbu")
            funct3 = "100";
        else if (ins == "lhu")
            funct3 = "101";
        else if (ins == "lwu")
            funct3 = "110";

        return bin_to_hex(imm + rs1 + funct3 + rd + opcode);
    }
    // for jalr instruction
    else if (ins == "jalr")
    {
        opcode = "1100111";
        funct3 = "000";
        s_index = instruction.find("(");
        if(s_index==string::npos){
            return "Error: Invalid Format";
        }
        imm = instruction.substr(0, s_index); // stores immediate value in string
        if(imm.empty()==true){
            return "Error: Instruction Expects 3 Arguments";
        }
        imm = immediate_binary(imm, 12);      // Find imm value in binary format by calling immediate_binary fun and passing size as 12 bit
        // if imm is not a valid value 
        if(imm.find("Error")!=string::npos){
            return imm;
        }
        instruction.erase(0, s_index + 1);
        s_index = instruction.find(")");
        if(s_index==string::npos){
            return "Error: Invalid Format";
        }
        rs1 = regname_to_binary(instruction.substr(0, s_index)); // stores source register in binary form
        if(rs1.find("Error")!=string::npos){
            return rs1;
        }

        return bin_to_hex(imm + rs1 + funct3 + rd + opcode);
    }
    
    else 
        return "Error: Not a valid I-format Instruction";
}

