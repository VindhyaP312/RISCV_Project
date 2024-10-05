#include <iostream>
#include <fstream>
#include <string>
#include <algorithm> // for reversing string during deci_to_bin conversion
#include <cmath>      // for 'pow' function
#include "riscv_header.h"
using namespace std;

string I_format(string instruction)
{
    vector<string> v = parse(instruction);
    if(v[0].find("Error")!=string::npos){
        return v[0];
    }
    string ins = v[1], rd = v[2], rs1 = v[3], imm = v[5], funct3, funct6 = "000000", opcode;
    if(rd.find("Error")!=string::npos){
        return rd;
    }
    if (ins == "addi" || ins == "xori" || ins == "ori" || ins == "andi")
    {
        opcode = "0010011";
        if(rs1.find("Error")!=string::npos){
            return rs1;
        }
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
        if(rs1.find("Error")!=string::npos){
            return rs1;
        }
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
        if(imm.empty()==true){
            return "Error: Instruction Expects 3 Arguments";
        }
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
        if(imm.empty()==true){
            return "Error: Instruction Expects 3 Arguments";
        }
        imm = immediate_binary(imm, 12);      // Find imm value in binary format by calling immediate_binary fun and passing size as 12 bit
        // if imm is not a valid value 
        if(imm.find("Error")!=string::npos){
            return imm;
        }
        if(rs1.find("Error")!=string::npos){
            return rs1;
        }

        return bin_to_hex(imm + rs1 + funct3 + rd + opcode);
    }
    
    else 
        return "Error: Not a valid I-format Instruction";
}


int I_execute(vector<string> part){
    string ins = part[1], rd = part[2], rs1 = part[3], imm = part[5];
    long int imm_value ; 
    // storing imm value given either in decimal or hexadecimal as long int 
    if(part[5][0] == '0' && (part[5][1] == 'x' || part[5][1] == 'X') || part[5][0] == '-' && part[5][1] == '0' && (part[5][2] == 'x' || part[5][2] == 'X')){
        imm_value = stol(part[5],nullptr,16);
    }
    else{
        imm_value = stol(part[5]);
    }
      // destination register is zero but instruction is not jalr type
    if(rd == "00000" && ins != "jalr")  {
        return 1;
    }
    if(ins == "addi")         reg_value[rd] = reg_value[rs1] + imm_value;
    else if(ins == "xori")    reg_value[rd] = reg_value[rs1] ^ imm_value;
    else if(ins == "ori")     reg_value[rd] = reg_value[rs1] | imm_value;
    else if(ins == "andi")    reg_value[rd] = reg_value[rs1] & imm_value;
    else if(ins == "slli"){
        reg_value[rd] = reg_value[rs1] << imm_value;
    }   
    else if(ins == "srli"){   
        unsigned long int srli_result = reg_value[rs1] >> imm_value; 
        unsigned long int shift_deci = (1ULL << (64 - imm_value)) - 1;
        srli_result = srli_result & shift_deci;
        reg_value[rd] = srli_result;
    }
    else if(ins == "srai") {
        reg_value[rd] = reg_value[rs1] >> imm_value;
    }   
    /*
    For loading any value from memory to register, depending on the type of instruction it reads that many characters from memory 
    Now this hex string should be made to 16 hex digits by inserting 0's or f's for signed loads
    For unsigned we rae inserting only 0's to make into 16 character sized string
    Now calling hex_to_deci fn, it returns long int value that to be stored in destination register
    If any particular memory is not initialised till then, it just takes them as 0's
    */
    else if(ins == "lb"){
        long int addr = reg_value[rs1] + imm_value;
        long int rel_addr = addr - stol("0x10000",nullptr,16);
        string mem_value_hex = "";
        for(int i=0;i<1;i++){
            char rightChar = '0', leftChar = '0';
            auto it = data_mem.find(2 * rel_addr + 1);
            if(it != data_mem.end()){
                leftChar = data_mem[2 * rel_addr + 1];
            }
            it = data_mem.find(2 * rel_addr);
            if(it != data_mem.end()){
                rightChar = data_mem[2 * rel_addr];
            }
            mem_value_hex.insert(mem_value_hex.begin(),rightChar);
            mem_value_hex.insert(mem_value_hex.begin(),leftChar);
            rel_addr++;
        }
        // inserting 0's
        if(mem_value_hex[0] >= '0' && mem_value_hex[0] <= '7' ){
            mem_value_hex.insert(mem_value_hex.begin(),14,'0');
        }
        // inserting f's
        else{
            mem_value_hex.insert(mem_value_hex.begin(),14,'f');
        }
        reg_value[rd] = hex_to_deci(mem_value_hex);
    }
    else if(ins == "lh"){
        long int addr = reg_value[rs1] + imm_value;
        long int rel_addr = addr - stol("0x10000",nullptr,16);
        string mem_value_hex = "";
        for(int i=0;i<2;i++){
            char rightChar = '0', leftChar = '0';
            auto it = data_mem.find(2 * rel_addr + 1);
            if(it != data_mem.end()){
                leftChar = data_mem[2 * rel_addr + 1];
            }
            it = data_mem.find(2 * rel_addr);
            if(it != data_mem.end()){
                rightChar = data_mem[2 * rel_addr];
            }
            mem_value_hex.insert(mem_value_hex.begin(),rightChar);
            mem_value_hex.insert(mem_value_hex.begin(),leftChar);
            rel_addr++;
        }
        // inserting 0's
        if(mem_value_hex[0] >= '0' && mem_value_hex[0] <= '7' ){
            mem_value_hex.insert(mem_value_hex.begin(),12,'0');
        }
        // inserting f's
        else{
            mem_value_hex.insert(mem_value_hex.begin(),12,'f');
        }
        reg_value[rd] = hex_to_deci(mem_value_hex);
    }
    else if(ins == "lw"){
        long int addr = reg_value[rs1] + imm_value;
        long int rel_addr = addr - stol("0x10000",nullptr,16);
        string mem_value_hex = "";
        for(int i=0;i<4;i++){
            char rightChar = '0', leftChar = '0';
            auto it = data_mem.find(2 * rel_addr + 1);
            if(it != data_mem.end()){
                leftChar = data_mem[2 * rel_addr + 1];
            }
            it = data_mem.find(2 * rel_addr);
            if(it != data_mem.end()){
                rightChar = data_mem[2 * rel_addr];
            }
            mem_value_hex.insert(mem_value_hex.begin(),rightChar);
            mem_value_hex.insert(mem_value_hex.begin(),leftChar);
            rel_addr++;
        }
        if(mem_value_hex[0] >= '0' && mem_value_hex[0] <= '7' ){
            mem_value_hex.insert(mem_value_hex.begin(),8,'0');
        }
        else{
            mem_value_hex.insert(mem_value_hex.begin(),8,'f');
        }
        reg_value[rd] = hex_to_deci(mem_value_hex);
    }
    else if(ins == "ld"){  // ld rd, imm(rs1) i.e, have to load 16 hex characters from given addr
        // rs1 has the address location(hexadecimal format like 0x10050) in long int format 
        long int addr = reg_value[rs1] + imm_value;
        long int rel_addr = addr - stol("0x10000",nullptr,16);
        // mem-value-hex is 16 char hexadecimal value that should be loaded into rd reg(long int)
        string mem_value_hex = "";
        for(int i=0;i<8;i++){
            char rightChar = '0', leftChar = '0';
            auto it = data_mem.find(2 * rel_addr + 1);
            if(it != data_mem.end()){
                leftChar = data_mem[2 * rel_addr + 1];
            }
            it = data_mem.find(2 * rel_addr);
            if(it != data_mem.end()){
                rightChar = data_mem[2 * rel_addr];
            }
            mem_value_hex.insert(mem_value_hex.begin(),rightChar);
            mem_value_hex.insert(mem_value_hex.begin(),leftChar);
            rel_addr++;
        }
        reg_value[rd] = hex_to_deci(mem_value_hex);
    }
    else if(ins == "lbu"){
        long int addr = reg_value[rs1] + imm_value;
        long int rel_addr = addr - stol("0x10000",nullptr,16);
        string mem_value_hex = "";
        for(int i=0;i<1;i++){
            char rightChar = '0', leftChar = '0';
            auto it = data_mem.find(2 * rel_addr + 1);
            if(it != data_mem.end()){
                leftChar = data_mem[2 * rel_addr + 1];
            }
            it = data_mem.find(2 * rel_addr);
            if(it != data_mem.end()){
                rightChar = data_mem[2 * rel_addr];
            }
            mem_value_hex.insert(mem_value_hex.begin(),rightChar);
            mem_value_hex.insert(mem_value_hex.begin(),leftChar);
            rel_addr++;
        }
        // inserting 0's since unsigned
        mem_value_hex.insert(mem_value_hex.begin(),14,'0');
        reg_value[rd] = hex_to_deci(mem_value_hex);
    }
    else if(ins == "lhu"){
        long int addr = reg_value[rs1] + imm_value;
        long int rel_addr = addr - stol("0x10000",nullptr,16);
        string mem_value_hex = "";
        for(int i=0;i<2;i++){
            char rightChar = '0', leftChar = '0';
            auto it = data_mem.find(2 * rel_addr + 1);
            if(it != data_mem.end()){
                leftChar = data_mem[2 * rel_addr + 1];
            }
            it = data_mem.find(2 * rel_addr);
            if(it != data_mem.end()){
                rightChar = data_mem[2 * rel_addr];
            }
            mem_value_hex.insert(mem_value_hex.begin(),rightChar);
            mem_value_hex.insert(mem_value_hex.begin(),leftChar);
            rel_addr++;
        }
        // inserting 0's since unsigned
        mem_value_hex.insert(mem_value_hex.begin(),12,'0');
        reg_value[rd] = hex_to_deci(mem_value_hex);
    }
    else if(ins == "lwu"){
        long int addr = reg_value[rs1] + imm_value;
        long int rel_addr = addr - stol("0x10000",nullptr,16);
        string mem_value_hex = "";
        for(int i=0;i<4;i++){
            char rightChar = '0', leftChar = '0';
            auto it = data_mem.find(2 * rel_addr + 1);
            if(it != data_mem.end()){
                leftChar = data_mem[2 * rel_addr + 1];
            }
            it = data_mem.find(2 * rel_addr);
            if(it != data_mem.end()){
                rightChar = data_mem[2 * rel_addr];
            }
            mem_value_hex.insert(mem_value_hex.begin(),rightChar);
            mem_value_hex.insert(mem_value_hex.begin(),leftChar);
            rel_addr++;
        }
        // inserting 0's since unsigned
        mem_value_hex.insert(mem_value_hex.begin(),8,'0');
        reg_value[rd] = hex_to_deci(mem_value_hex);
    }
    else if(ins == "jalr"){     // jalr rd, imm(rs1)
    // updates call stack by popping the top most function call 
    if(!call_stack.empty())
        call_stack.pop_back();
    long int jump_count = reg_value[rs1] + imm_value;  // jump addr in long int type 
    if(rd != "00000")
        reg_value[rd] = (lineNo + 1)*4;   
    // returns relative offset i.e, count by which it has to jump
    return (jump_count - 4*lineNo )/4; 
    }

    return 1;
}