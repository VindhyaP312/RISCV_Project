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
string J_format(string instruction)    // jal x5, 108
{
    vector<string> v = parse(instruction);
    if(v[0].find("Error")!=string::npos){
        return v[0];
    }
    string ins = v[1], rd = v[2], imm = v[5], opcode = "1101111";
    // if any register name is not a valid register name, it returns the error message that regname_to_binary function gave
    if(rd.find("Error")!=string::npos){
        return rd;
    }
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


int J_execute(vector<string> part){
    string rd = part[2];
    int i = 0, imm;
    // converting imm string to long int value
    // for hexa decimal and label names
    if (!(all_of(part[5].begin(), part[5].end(), ::isdigit) || (part[5][0] == '-' && all_of(part[5].begin() + 1, part[5].end(), ::isdigit)))){
        // for hexadecimal
        if(part[5][0] == '0' && (part[5][1] == 'x' || part[5][1] == 'X') || part[5][0] == '-' && part[5][1] == '0' && (part[5][2] == 'x' || part[5][2] == 'X')){
            imm = stol(part[5], nullptr, 16);
        }
        // for labels
        else{
            for (i = 0; i < label.size(); i++)
            {
                if (label[i].name == part[5])
                { // searching for the first occurence of that label from vector 'label'
                    break;
                }
            }
            imm = (label[i].line_no - lineNo)*4;
            // pushing this label i.e, the fuction name that is going to be executed now along with its line number of that label
            call_stack.push_back({part[5],label[i].line_no});
        }
    }
    // for direct decimal values
    else{
        imm = stol(part[5]);
    }
    // dest reg should not be updated if it is x0
    if(rd != "00000"){
        reg_value[rd] = (lineNo + 1)*4;
    }
    return imm/4;
}