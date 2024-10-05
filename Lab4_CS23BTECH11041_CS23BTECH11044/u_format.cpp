#include <iostream>
#include <fstream>
#include <string>
#include <algorithm> // for reversing string during deci_to_bin conversion
#include <cmath>      // for 'pow' function
using namespace std;
#include "riscv_header.h"

/* takes the whole insruction string as argument and returns 8 bit hexcode or any error message
* does string concatenation of binary codes of registers, imm and opcode in correct order
* calls bin_to_hex fn, which converts binary code to hexcode
*/
string U_format(string instruction)    // lui x3, 0x100
{
    vector<string> v = parse(instruction);
    if(v[0].find("Error")!=string::npos){
        return v[0];
    }
    string ins = v[1], rd = v[2], imm = v[5], opcode = "0110111";
    // if any register name is not a valid register name, it returns the error message that regname_to_binary function gave
    if(rd.find("Error")!=string::npos){
        return rd;
    }
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

int U_execute(vector<string> part){
    string rd = part[2], imm;
    // should not be overwritten for x0
    if(rd == "00000"){
        return 1;
    }
    // hexa decimal imm value
    if(part[5][0] == '0' && (part[5][1] == 'x' || part[5][1] == 'X')){
        imm = part[5];
        imm.erase(0,2);
        imm.insert(imm.begin(),5-imm.length(),'0');
        imm.insert(imm.end(),3,'0');
        imm = "0x" + imm;
    }
    else{
        imm = deci_to_hex(part[5],5);
        imm.insert(imm.end(),3,'0');
    }

    reg_value[rd] = stol(imm, nullptr,16);
    return 1;
}