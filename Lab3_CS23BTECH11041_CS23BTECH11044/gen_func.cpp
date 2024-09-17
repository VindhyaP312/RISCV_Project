#include <iostream>
#include <fstream>
#include <string>
#include <algorithm> // for reversing string during deci_to_bin conversion
#include <cmath>     // for 'pow' function
#include <vector>
using namespace std;
#include "assembler.h"

/*
It takes the instruction name as argument like 'add'
and returns the format type which is 'R' for add
If no instruction with that name exists, it returns zero
*/
char findformat(string name)
{
    if (name == "add" || name == "sub" || name == "and" || name == "or" || name == "xor" || name == "sll" || name == "srl" || name == "sra")
        return 'R';
    else if( name == "addi" || name == "andi" || name == "ori" || name == "xori" || name == "slli" || name == "srli" || name == "srai" || name == "ld" || name == "lb" || name == "lh" || name == "lw" || name == "lbu" || name == "lhu" || name == "lwu" || name == "jalr")
        return 'I';
    else if(name == "sb" || name == "sh" || name == "sw" || name == "sd")
        return 'S';
    else if(name == "beq" || name == "bne" || name == "blt" || name == "bge" || name == "bltu" || name == "bgeu")
        return 'B';
    else if(name == "jal")
        return 'J';
    else if (name == "lui")
        return 'U';
    else 
        return '0';
}

/*
It converts 32 bit binarycode(bc) to 8 digit hexa decimal number.
It takes and returns in string format
Internally, it converts every 4 binary digits to one hexadecimal digit using 'for' loop
*/
string bin_to_hex(string bc)
{ 
    string hexcode;
    for (int i = 0; i <= 31; i += 4)
    {
        string hexdigits = "0123456789abcdef";
        string _4bits = string(1, bc[i]) + string(1, bc[i + 1]) + string(1, bc[i + 2]) + string(1, bc[i + 3]); 
        int decValue = (_4bits[0] - 48) * 8 + (_4bits[1] - 48) * 4 + (_4bits[2] - 48) * 2 + (_4bits[3] - 48) * 1;
        hexcode += string(1, hexdigits[decValue]);
    }
    return hexcode;
}

/*
Takes decimal number(dc) in string format and binary code size in which it has to fit in(size-sz)
It can take both postive and negative numbers
It returns 2's complement form of binary code with msb as sign bit
*/
string deci_to_bin(string dc, int sz) // dc- decimal code, sz- size of binary code it returns
{   
    int max_value = pow(2,sz-1) - 1;   // For positive numbers
    int min_value = -pow(2,sz-1);      // For negative numbers
    if (stoi(dc)> max_value || stoi(dc) < min_value) { // Checking if the number is within bounds
        return "Error: Out of Bound";
    }
    string bincode;
    int sign = 0;
    if (dc[0] == '-')
    {
        sign = 1;
        dc.erase(0, 1);   // erases the negative sign character
    }
    int decno = stoi(dc); // converts string to decimal number 
    for (int i = decno; i != 0;)
    {
        bincode += i % 2 + '0';
        i = i / 2;
    }
    while (bincode.length() < sz) {   // makes that 'sz' sized binary number
        bincode.push_back('0');
    }
    reverse(bincode.begin(), bincode.end()); // reverse the binary string
    if (sign == 1)     // For negative numbers, taking inversion and adding 1
    {
        int i;
        for (i = bincode.length() - 1; bincode[i] != '1'; i--){}
        for (i = i - 1; i >= 0; i--)
        {
            if (bincode[i] == '0')
                bincode[i] = '1';
            else
                bincode[i] = '0';
        }
    }
    return bincode;
}

/*
Here reg, takes x0-x31, some alias names and also manages if any any wrong register name is given
For alias names, it converts into proper register name first.  ex: 't1' to 'x6'
So now reg names are from x0 to x31 and some invalid names 
*/
string regname_to_binary(string reg)
{ 
    string regis = reg;
    if (reg[0] != 'x')
    {
        if (reg == "zero")
            reg = "x0";
        else if (reg == "ra")
            reg = "x1";
        else if (reg == "sp")
            reg = "x2";
        else if (reg == "gp")
            reg = "x3";
        else if (reg == "tp")
            reg = "x4";
        else if (reg == "t0")
            reg = "x5";
        else if (reg == "t1")
            reg = "x6";
        else if (reg == "t2")
            reg = "x7";
        else if (reg == "s0" || reg == "fp")
            reg = "x8";
        else if (reg == "s1")
            reg = "x9";
        else if (reg[0] == 'a')
        {
            reg[0] = 'x';
            char temp = reg[1];
            if (reg.substr(1,reg.length()-1).empty() || !all_of(reg.begin()+1, reg.end(), ::isdigit)){
                return "Error: Invalid Register: "+regis;
            }
            reg[1] = '1';
            reg += temp; 
        }
        else if (reg[0] == 's')   // ex: reg = "s4"
        {
            reg.erase(0, 1);      // reg = "4"  
            if (reg.empty() || !all_of(reg.begin()+1, reg.end(), ::isdigit)){
                return "Error: Invalid Register: "+regis;
            }
            int temp = stoi(reg);      // temp = 4
            temp = temp + 16;          //  temp = 20
            reg.clear();               // reg is empty now
            reg += 'x';                // reg = "x"
            reg += (temp / 10) + '0';  // reg = "x2"
            reg += (temp % 10) + '0';  // reg = "x20"  (Appending each digit)
        }
        else if (reg[0] == 't')
        {
            reg.erase(0, 1);
            if (reg.empty() || !all_of(reg.begin()+1, reg.end(), ::isdigit)){
                return "Error: Invalid Register: "+regis;
            }
            int temp = stoi(reg);
            temp = temp + 25;
            reg.clear();
            reg += 'x';
            reg += (temp / 10) + '0';
            reg += (temp % 10) + '0';
        }
    }
    if (reg[0] != 'x'){     // for invalid names not starting with 'x'
        return "Error: Invalid Register: "+ regis;
    }
    reg.erase(0, 1);   // removes the letter x
    if (reg.empty() || !all_of(reg.begin(), reg.end(), ::isdigit))    // takes care of regs like xab, xa1 etc 
    {
        return "Error: Invalid Register: "+ regis;
    }
    /* the reg in string format which only conatins number right now is sent to deci_to_bin function 
    and it returns the binary code in 6 bits*/
    reg = deci_to_bin(reg,6);
    
    if(reg.find("Error")!=string::npos){     // If the register is not in range 0-31, it gives out of bound error
        return "Error: Invalid Register: "+ regis;
    }
    reg.erase(0, 1);  //  erasing the msb bit which was zero

    return reg;
}

/* It takes hexadecimal string and size of binary string it has to convert to as arguments
* Returns binary code of specified size if given number fits in that size o/w gives 'Out of bound error'
* Passed hexadecimal number can be anything of these type, ex: 0x12A or 0X12A or -0x12A or -0X12A
* If given is invalid hexadecimal, then it gives 'Invalid hexadecimal error'
*/
string hex_to_bin(string s,int sz){   
    int dec=0,i=0;
    if(s[0]!='-'){     // for +ve 
        s.erase(0,2);
        for(i=0; i<s.length(); i++){
            if(s[i]=='A' || s[i]=='B' || s[i]=='C' || s[i]=='D' || s[i]=='E' || s[i]=='F')
                dec += (s[i]-'A' + 10)*pow(16,s.length()-1-i);
            else if(s[i]=='a' || s[i]=='b' || s[i]=='c' || s[i]=='d' || s[i]=='e' || s[i]=='f')
                dec += (s[i]-'a' + 10)*pow(16,s.length()-1-i);
            else if(isdigit(s[i])==true )   
                dec += (s[i]-'0')*pow(16,s.length()-1-i); 
            else
                return "Error: Invalid Hexadecimal ";
        }
    }
    
    else {       // for -ve
        s.erase(0,3);
        for(i=0; i<s.length(); i++){
            if(s[i]=='A' || s[i]=='B' || s[i]=='C' || s[i]=='D' || s[i]=='E' || s[i]=='F')
                dec += (s[i]-'A' + 10)*pow(16,s.length()-1-i);
            else if(s[i]=='a' || s[i]=='b' || s[i]=='c' || s[i]=='d' || s[i]=='e' || s[i]=='f')
                dec += (s[i]-'a' + 10)*pow(16,s.length()-1-i);
            else if(isdigit(s[i])==true )   
                dec += (s[i]-'0')*pow(16,s.length()-1-i); 
            else
                return "Error: Invalid Hexadecimal "; 
        }
        dec *= -1;
    }
    s = to_string(dec);
    s = deci_to_bin(s,sz);  
    return s;
}

/* It takes immediate value as string and size of binary string it has to convert to as arguments
* Returns binary code of specified size if given number fits in that size o/w gives error msg
* Passed immediate can be +ve or -ve decimal or hexadecimal numbers
* If given is invalid immediate value, then it gives 'Invalid Immediate error'
*/
string immediate_binary(string s, int sz){
    int sign=0;
    if(s[0]=='-') 
        sign = 1;
    // for hexadecimal
    if((s[0]=='0' && (s[1]=='x' || s[1]=='X')) || s[0] == '-' && s[1]=='0' && (s[2]=='x' || s[2]=='X')){   // 0x12A or 0X12A or -0x12A or -0X12A
        s = hex_to_bin(s,sz);
        if(s.find("Error")!=string::npos){
            return s;
        }
    }
    // for decimal
    else if(all_of(s.begin(), s.end(), ::isdigit)|| s[0]=='-' && all_of(s.begin()+1, s.end(), ::isdigit))
        s = deci_to_bin(s,sz); 
    else{
        return "Error: Invalid Immediate";
    }
    
    if(s.find("Error")!=string::npos){
        return "Error: Immediate value does not fit in " + to_string(sz) + " bits " ;
    }

    return s;
}

