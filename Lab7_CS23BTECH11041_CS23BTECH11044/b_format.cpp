#include <iostream>
#include <fstream>
#include <string>
#include <algorithm> // for reversing string during deci_to_bin conversion
#include <cmath>      // for 'pow' function
using namespace std;
#include "riscv_header.h"

/* takes the whole insruction string as argument and returns 8 bit hexcode or any error message
* does string concatenation of binary codes of registers, funct3, imm and opcode in correct order
* calls bin_to_hex fn, which converts binary code to hexcode
*/
string B_format(string instruction)
{
    vector<string> v = parse(instruction);
    if(v[0].find("Error")!=string::npos){
        return v[0];
    }
    string ins = v[1], rs2 = v[4], rs1 = v[3], imm = v[5], funct3, opcode = "1100011";
    // if any register name is not a valid register name, it returns the error message that regname_to_binary function gave
    if(rs1.find("Error")!=string::npos){
        return rs1;
    }
    if(rs2.find("Error")!=string::npos){
        return rs2;
    }
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

/* Takes the parsed instruction and does those comparisions of register values
If comparison is true for that instruction then returns the offset to which it has to jump i.e,
it returns the relative line number that has to be executed next.
*/
int B_execute(vector<string> part){
    string ins = part[1], rs1 = part[3], rs2 = part[4];
    int imm;
    int i = 0;
    if (!(all_of(part[5].begin(), part[5].end(), ::isdigit) || (part[5][0] == '-' && all_of(part[5].begin() + 1, part[5].end(), ::isdigit)))){
        // hexadecimal
        if(part[5][0] == '0' && (part[5][1] == 'x' || part[5][1] == 'X') || part[5][0] == '-' && part[5][1] == '0' && (part[5][2] == 'x' || part[5][2] == 'X')){
            imm = stol(part[5],nullptr,16);
        }
        // label name
        else{
            for (i = 0; i < label.size(); i++)
            {
                if (label[i].name == part[5])
                { // searching for the first occurence of that label from vector 'label'
                    break;
                }
            }
            imm = (label[i].line_no - lineNo)*4;
        }
    }
    // direct integer value
    else{
        imm = stol(part[5]);
    }

    //in the case where the imm is not given as a multiple of 4, the /4 truncates it anyways
    if(ins == "beq"){
        if(reg_value[rs1] == reg_value[rs2])
            return imm/4;
        else
            return 1;
    }
    else if(ins == "bne"){
        if(reg_value[rs1] != reg_value[rs2])
            return imm/4;
        else
            return 1;
    }   
    else if(ins == "blt"){
        if(reg_value[rs1] < reg_value[rs2])
            return imm/4;
        else
            return 1;
    } 
    else if(ins == "bge"){
        if(reg_value[rs1] >= reg_value[rs2])
            return imm/4;
        else
            return 1;
    } 
    else if(ins == "bltu"){
        // converting to unsigned long ints and comparing them
        unsigned long temp1 = reg_value[rs1];
        unsigned long temp2 = reg_value[rs2];
        if(temp1 < temp2)
            return imm/4;
        else
            return 1;
    } 
    else if(ins == "bgeu"){
        // // converting to unsigned long ints and comparing them
        unsigned long temp1 = reg_value[rs1];
        unsigned long temp2 = reg_value[rs2];
        if(temp1 >= temp2)
            return imm/4;
        else
            return 1;
    }
    else{
        return 1;
    } 
}