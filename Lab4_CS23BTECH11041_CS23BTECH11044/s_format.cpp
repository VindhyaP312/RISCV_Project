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
string S_format(string instruction)
{
    vector<string> v = parse(instruction);
    if(v[0].find("Error")!=string::npos){
        return v[0];
    }
    string ins = v[1], rs2 = v[4], rs1 = v[3], imm = v[5], funct3, opcode = "0100011";
    // if any register name is not a valid register name, it returns the error message that regname_to_binary function gave
    if(rs2.find("Error")!=string::npos){ 
        return rs2;
    }
    if(imm.empty()==true){
        return "Error: Instruction Expects 3 Arguments";
    }
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


int S_execute(vector<string> part){ // sd rs2, imm(rs1)
    string ins = part[1], rs1 = part[3], rs2 = part[4], imm = part[5];
    long int imm_value ;
    if((imm[0] == '0' && (imm[1] == 'x' || imm[1] == 'X')) || (imm[0] == '-' && imm[1] == '0' && (imm[2] == 'x' || imm[2] == 'X'))){
        imm_value = stol(part[5],nullptr,16);
    }
    else{
        imm_value = stol(imm);
    }

    if(ins == "sb"){
        long int addr = reg_value[rs1] + imm_value;
        long int rel_addr = addr - stol("0x10000",nullptr,16);
        string rs2_hex = deci_to_hex(to_string(reg_value[rs2]),2);
        for (int i = 1; i >= 0; i--)
        {
            // push these hex digits to data section
            if(rel_addr < 0){
                load_error = -2;
                return 1;
            }
            data_mem[2*rel_addr+1-i] = rs2_hex[i];
        }
    }    
    else if(ins == "sh"){
        long int addr = reg_value[rs1] + imm_value;
        long int rel_addr = addr - stol("0x10000",nullptr,16);
        string rs2_hex = deci_to_hex(to_string(reg_value[rs2]),4);
        for (int i = 3; i >= 0; i--)
        {
            // push these hex digits to data section
            if(rel_addr < 0){
                load_error = -2;
                return 1;
            }
            data_mem[2*rel_addr+3-i] = rs2_hex[i];
        }
    }   
    else if(ins == "sw") {
        long int addr = reg_value[rs1] + imm_value;
        long int rel_addr = addr - stol("0x10000",nullptr,16);
        string rs2_hex = deci_to_hex(to_string(reg_value[rs2]),8);
        for (int i = 7; i >= 0; i--)
        {
            // push these hex digits to data section
            if(rel_addr < 0){
                load_error = -2;
                return 1;
            }
            data_mem[2*rel_addr+7-i] = rs2_hex[i];
        }
    }   
    else if(ins == "sd"){
        long int addr = reg_value[rs1] + imm_value;
        long int rel_addr = addr - stol("0x10000",nullptr,16);
        // rs2 has long int which should be stored in data memory in hex 
        // rs2_hex has 16 hex chars 
        string rs2_hex = deci_to_hex(to_string(reg_value[rs2]),16);
        for (int i = 15; i >= 0; i--)
        {
            // push these hex digits to data section
            if(rel_addr < 0){
                load_error = -2;
                return 1;
            }
            data_mem[2*rel_addr+15-i] = rs2_hex[i];
        }
    }     
    
    return 1;
}