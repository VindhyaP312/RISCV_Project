#include <iostream>
#include <fstream>
#include <string>
#include <algorithm> // for reversing string during deci_to_bin conversion
#include <cmath>     // for 'pow' function
#include <vector>
#include <map> // to store registers and its value
#include <iomanip> 
using namespace std;
#include "riscv_header.h"

// initializing text memory with hex codes by reading from output.hex file
void initializeTextMem(){
    ifstream in("output.hex");     // opens the file to read
    string hexcode;
    int j=0;
    while(getline(in,hexcode)){
        for (int i = 7; i >= 0; i--)
        {
            text_mem[j]=hexcode[i];
            j++;
        }
    }
    in.close();  // closing the output.hex file
}

// initializing reg values with zeroes 
void initialize_reg_values()
{
    string reg;
    for (int i = 0; i < 32; i++)
    {
        reg = (deci_to_bin(to_string(i), 6)).erase(0, 1);
        reg_value[reg] = 0;
    }
}

// prints reg values in proper format in hexadecimal values by converting long it to hexa
void print_reg_values()
{
    cout << "Registers:" << endl;
    string reg, deci_reg_val;
    for (int i = 0; i < 32; i++)
    {
        reg = (deci_to_bin(to_string(i), 6)).erase(0, 1);
        deci_reg_val = to_string(reg_value[reg]);
        string regValue = deci_to_hex(deci_reg_val, 16);
        int nonZeroIdx = regValue.find_first_not_of('0');
        if (nonZeroIdx == string::npos)
            regValue = "0";
        else
            regValue.erase(0, nonZeroIdx);
        cout << left << setw(4) << ("x" + to_string(i)) << "= 0x" << regValue << endl;
    }
    cout << endl;
}

// prints fn calls stored in vector along with last executed line number
void print_call_stack(){
    if(lineNo >= ins_lines.size() || call_stack.empty()){
        cout << "Empty Call Stack: Execution complete" << endl;
        return ;
    }
    cout << "Call Stack:" << endl;
    for (const auto& call : call_stack) {
        cout << call.fn_name << ":" << call.fn_last_line + data_line_count  << endl;
    }
}

// prints memory values from both text and data section depending on the address and count provided
void print_memory_values(string addr, int count)
{
    if(count <= 0 ){
        cout << "Invalid count value. Count value should be non-negative" << endl << endl;
        return ;
    }
    long int address = stol(addr, nullptr, 16);
    for (int i = 0; i < count; i++)
    {    
        if (address < stol("0x10000", nullptr, 16))
        { // i.e, text section
            char rightChar = '0', leftChar = '0';
            auto it = text_mem.find(2 * address + 1);
            if(it != text_mem.end()){
                leftChar = text_mem[2 * address + 1];
            }
            it = text_mem.find(2 * address);
            if(it != text_mem.end()){
                rightChar = text_mem[2 * address];
            }
            cout << "Memory[0x" << deci_to_hex(to_string(address), 5) << "] = 0x" ;
            if(rightChar == '0' && leftChar == '0'){
                cout << '0' << endl;
            }
            else{
                cout << leftChar << rightChar << endl;
            }
            address += 1;
        }
        else if (address >= stol("0x10000", nullptr, 16))
        {       // i.e, data section
            int rel_addr; // relative address
            rel_addr = address - stol("0x10000", nullptr, 16);
            char rightChar = '0', leftChar = '0';
            auto it = data_mem.find(2 * rel_addr + 1);
            if(it != data_mem.end()){
                leftChar = data_mem[2 * rel_addr + 1];
            }
            it = data_mem.find(2 * rel_addr);
            if(it != data_mem.end()){
                rightChar = data_mem[2 * rel_addr];
            }
            cout << "Memory[0x" << deci_to_hex(to_string(address), 5) << "] = 0x" ;
            if(rightChar == '0' && leftChar == '0'){
                cout << '0' << endl;
            }
            else{
                cout << leftChar << rightChar << endl;
            }
            address += 1;
            
        }
        else
        {
            cout << "Error : Incorrect memory address" << endl;
        }
    }

    cout << endl;
}

void step()
{
    if(lineNo == ins_lines.size()){
        cout << "Nothing to step" << endl;
        return ;
    }
    call_stack[call_stack.size()-1].fn_last_line = lineNo + 1;
    string instruction = ins_lines[lineNo];
    // in instruction remove L1: if exists"
    if (instruction.find(':') != string::npos)
        instruction.erase(0, instruction.find(':') + 2);
    vector<string> v = parse(instruction);
    char format_type = findformat(v[1]);
    int rel_jump = 0; // relative jump
    switch (format_type)
    {
    case 'R':
        rel_jump = R_execute(v);
        break;
    case 'I':
        rel_jump = I_execute(v);
        break;
    case 'S':
        rel_jump = S_execute(v);
        break;
    case 'B':
        rel_jump = B_execute(v);
        break;
    case 'J':
        rel_jump = J_execute(v);
        break;
    case 'U':
        rel_jump = U_execute(v);
        break;
    }

    cout << "Executed " << instruction << "; PC=0x" << deci_to_hex(to_string(lineNo * 4), 8) << endl;
    if(load_error == -2){
        cout<<"Error: Code is writing to read-only memory. Please load another file."<<endl<<endl;
        return ;
    }
    lineNo += rel_jump;
    if(lineNo<0 || lineNo>ins_lines.size()){
        load_error = -3;
        cout<<"Error: Invalid instruction address"<<endl<<endl;
        return;
    }
}

// calls step continuously till it reaches a break point or till it executes all instructions
void run()
{
    while (lineNo != ins_lines.size())
    {
        auto it = break_points.find(lineNo);
        if( it != break_points.end() && it -> second == 0){    // if already that run flow has stopped there once
            cout << "Execution stopped at breakpoint" << endl;
            it -> second = 0;
            break;
        }
        if(it != break_points.end() && it -> second == 1){
            it -> second = 0;
        }
        step();
        if(load_error == -2){
            return ;
        }
        if(load_error == -3){
            return;
        }
    }
    cout << endl;
}