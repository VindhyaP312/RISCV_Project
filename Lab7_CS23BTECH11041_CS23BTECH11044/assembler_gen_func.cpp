#include <iostream>
#include <fstream>
#include <string>
#include <algorithm> // for reversing string during deci_to_bin conversion
#include <cmath>     // for 'pow' function
#include <vector>
using namespace std;
#include "riscv_header.h"
#include <sstream>
#include <iomanip>

/*
It takes the instruction name as argument like 'add'
and returns the format type which is 'R' for add
If no instruction with that name exists, it returns zero
*/
char findformat(string name)
{
    if (name == "add" || name == "sub" || name == "and" || name == "or" || name == "xor" || name == "sll" || name == "srl" || name == "sra")
        return 'R';
    else if (name == "addi" || name == "andi" || name == "ori" || name == "xori" || name == "slli" || name == "srli" || name == "srai" || name == "ld" || name == "lb" || name == "lh" || name == "lw" || name == "lbu" || name == "lhu" || name == "lwu" || name == "jalr")
        return 'I';
    else if (name == "sb" || name == "sh" || name == "sw" || name == "sd")
        return 'S';
    else if (name == "beq" || name == "bne" || name == "blt" || name == "bge" || name == "bltu" || name == "bgeu")
        return 'B';
    else if (name == "jal")
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

string bin_to_hex_sz(string bc, int sz)
{
    string hexcode = "";
    for (int i = 0; i <= 4*sz -1; i += 4)
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
    long int max_value = (1 << (sz - 1)) - 1; // For positive numbers
    long int min_value = -(1 << (sz - 1));   // For negative numbers
    if (stol(dc) > max_value || stol(dc) < min_value)
    { // Checking if the number is within bounds
        return "Error: Out of Bound";
    }
    string bincode = "";
    int sign = 0;
    if (dc[0] == '-')
    {
        sign = 1;
        dc.erase(0, 1); // erases the negative sign character
    }
    long int decno = stol(dc); // converts string to decimal number
    for (long int i = decno; i != 0;)
    {
        bincode += i % 2 + '0';
        i = i / 2;
    }
    while (bincode.length() < sz)
    { // makes that 'sz' sized binary number
        bincode.push_back('0');
    }
    reverse(bincode.begin(), bincode.end()); // reverse the binary string
    if (sign == 1)                           // For negative numbers, taking inversion and adding 1
    {
        int i;
        for (i = bincode.length() - 1; bincode[i] != '1'; i--)
        {
        }
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
            if (reg.substr(1, reg.length() - 1).empty() || !all_of(reg.begin() + 1, reg.end(), ::isdigit))
            {
                return "Error: Invalid Register: " + regis;
            }
            reg[1] = '1';
            reg += temp;
        }
        else if (reg[0] == 's') // ex: reg = "s4"
        {
            reg.erase(0, 1); // reg = "4"
            if (reg.empty() || !all_of(reg.begin() + 1, reg.end(), ::isdigit))
            {
                return "Error: Invalid Register: " + regis;
            }
            int temp = stoi(reg);     // temp = 4
            temp = temp + 16;         //  temp = 20
            reg.clear();              // reg is empty now
            reg += 'x';               // reg = "x"
            reg += (temp / 10) + '0'; // reg = "x2"
            reg += (temp % 10) + '0'; // reg = "x20"  (Appending each digit)
        }
        else if (reg[0] == 't')
        {
            reg.erase(0, 1);
            if (reg.empty() || !all_of(reg.begin() + 1, reg.end(), ::isdigit))
            {
                return "Error: Invalid Register: " + regis;
            }
            int temp = stoi(reg);
            temp = temp + 25;
            reg.clear();
            reg += 'x';
            reg += (temp / 10) + '0';
            reg += (temp % 10) + '0';
        }
    }
    if (reg[0] != 'x')
    { // for invalid names not starting with 'x'
        return "Error: Invalid Register: " + regis;
    }
    reg.erase(0, 1);                                               // removes the letter x
    if (reg.empty() || !all_of(reg.begin(), reg.end(), ::isdigit)) // takes care of regs like xab, xa1 etc
    {
        return "Error: Invalid Register: " + regis;
    }
    /* the reg in string format which only conatins number right now is sent to deci_to_bin function
    and it returns the binary code in 6 bits*/
    reg = deci_to_bin(reg, 6);

    if (reg.find("Error") != string::npos)
    { // If the register is not in range 0-31, it gives out of bound error
        return "Error: Invalid Register: " + regis;
    }
    reg.erase(0, 1); //  erasing the msb bit which was zero

    return reg;
}

/* It takes hexadecimal string and size of binary string it has to convert to as arguments
 * Returns binary code of specified size if given number fits in that size o/w gives 'Out of bound error'
 * Passed hexadecimal number can be anything of these type, ex: 0x12A or 0X12A or -0x12A or -0X12A
 * If given is invalid hexadecimal, then it gives 'Invalid hexadecimal error'
 */
string hex_to_bin(string s, int sz)
{
    int dec = 0, i = 0;
    if (s[0] != '-')
    { // for +ve
        s.erase(0, 2);
        for (i = 0; i < s.length(); i++)
        {
            if (s[i] == 'A' || s[i] == 'B' || s[i] == 'C' || s[i] == 'D' || s[i] == 'E' || s[i] == 'F')
                dec += (s[i] - 'A' + 10) * pow(16, s.length() - 1 - i);
            else if (s[i] == 'a' || s[i] == 'b' || s[i] == 'c' || s[i] == 'd' || s[i] == 'e' || s[i] == 'f')
                dec += (s[i] - 'a' + 10) * pow(16, s.length() - 1 - i);
            else if (isdigit(s[i]) == true)
                dec += (s[i] - '0') * pow(16, s.length() - 1 - i);
            else
                return "Error: Invalid Hexadecimal ";
        }
    }

    else
    { // for -ve
        s.erase(0, 3);
        for (i = 0; i < s.length(); i++)
        {
            if (s[i] == 'A' || s[i] == 'B' || s[i] == 'C' || s[i] == 'D' || s[i] == 'E' || s[i] == 'F')
                dec += (s[i] - 'A' + 10) * pow(16, s.length() - 1 - i);
            else if (s[i] == 'a' || s[i] == 'b' || s[i] == 'c' || s[i] == 'd' || s[i] == 'e' || s[i] == 'f')
                dec += (s[i] - 'a' + 10) * pow(16, s.length() - 1 - i);
            else if (isdigit(s[i]) == true)
                dec += (s[i] - '0') * pow(16, s.length() - 1 - i);
            else
                return "Error: Invalid Hexadecimal ";
        }
        dec *= -1;
    }
    s = to_string(dec);
    s = deci_to_bin(s, sz);
    return s;
}

/* It takes immediate value as string and size of binary string it has to convert to as arguments
 * Returns binary code of specified size if given number fits in that size o/w gives error msg
 * Passed immediate can be +ve or -ve decimal or hexadecimal numbers
 * If given is invalid immediate value, then it gives 'Invalid Immediate error'
 */
string immediate_binary(string s, int sz)
{
    int sign = 0;
    if (s[0] == '-')
        sign = 1;
    // for hexadecimal
    if ((s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) || s[0] == '-' && s[1] == '0' && (s[2] == 'x' || s[2] == 'X'))
    { // 0x12A or 0X12A or -0x12A or -0X12A
        s = hex_to_bin(s, sz);
        if (s.find("Error") != string::npos)
        {
            return s;
        }
    }
    // for decimal
    else if (all_of(s.begin(), s.end(), ::isdigit) || s[0] == '-' && all_of(s.begin() + 1, s.end(), ::isdigit))
        s = deci_to_bin(s, sz);
    else
    {
        return "Error: Invalid Immediate";
    }

    if (s.find("Error") != string::npos)
    {
        return "Error: Immediate value does not fit in " + to_string(sz) + " bits ";
    }

    return s;
}

/*
Takes hexadecimal string as argument without the starting 0x characters and converts to long int
It considers given hexadecimal string as 2s complement representation
So based on msb bit, it will convert to proper long int value
*/
long int hex_to_deci(string hc){
    transform(hc.begin(), hc.end(), hc.begin(), ::tolower);
    char msb = hc[0];
    string bin_value = "";
    string temp;
    long int deci_value = 0;
    if(msb == '0'|| msb == '1' || msb == '2' || msb == '3' || msb == '4' || msb == '5' || msb == '6' || msb == '7'){
        return stol(hc,nullptr,16);
    }
    else if( msb == '8' || msb == '9' || msb == 'a' || msb == 'b' || msb == 'c' || msb == 'd' || msb == 'e' || msb == 'f'){
        unsigned long int value = stoul(hc, nullptr, 16);
        long int signed_value = static_cast<long int>(value);

        // If the number is negative, adjust to a signed value
        if (signed_value < 0) {
            deci_value = signed_value;
        } else {
            // Convert unsigned to signed explicitly
            deci_value = static_cast<long int>(value - (1ULL << 63));
        }

        return deci_value;
    }
    else{
        cout << "Error: in hex_to_deci fn" << endl;
        return -1;
    }

}


/*
It converts given decimal string(can be long int in string format), it converts to hexadecimal
second argumnt size helps in making hexadecimal string of that many characters
Used in converting pc value with sz =8
while storing register values into memory with sz = 16
*/
string deci_to_hex(string dc, int sz) {
    // Create a stringstream for conversion to hexadecimal
    stringstream ss;

    // Convert the long integer to hexadecimal with leading zeros, 
    // and handle two's complement for negative numbers.
    // setw and setfill will do ony when less characters than size specified
    ss << hex << setw(sz) << setfill('0') << (stol(dc) & 0xFFFFFFFFFFFFFFFF);

    // Get the resulting hex string
    string hex_string = ss.str();

    // If the resulting string is longer than 'sz', return only the last 'sz' characters
    if (hex_string.length() > sz) {
        hex_string = hex_string.substr(hex_string.length() - sz);
    }

    return hex_string;
}

// part[0]=format error, part[1]=ins, part[2]=rd, part[3]=rs1, part[4]=rs2, part[5]=imm
// parts -2,3,4 contains binary format of those registers, part-5 contains in integer/hexadecimal itself but in string format or label names for branch instrcutions
// for an instruction, if some parts are not present, then it just has empty string
vector<string> parse(string instruction)
{
    int s_index = 0;
    vector<string> part(6, ""); // all parts initialized to empty string
    s_index = instruction.find(" ");
    part[1] = instruction.substr(0, s_index); // stores the name of the instruction
    if (part[1] == "add" || part[1] == "sub" || part[1] == "and" || part[1] == "or" || part[1] == "xor" || part[1] == "sll" || part[1] == "srl" || part[1] == "sra")
    {
        //  r-type has 3 arguments, so should have 2 commas
        if (std::count(instruction.begin(), instruction.end(), ',') != 2)
        {
            part[0] = "Error: Instruction expects 3 arguments ";
            return part;
        }
        //   returns error string if space is not present b/w comma and next argument
        if (std::count(instruction.begin(), instruction.end(), ' ') != 3)
        {
            part[0] = "Error: Incorrect Format ";
            return part;
        }
        instruction.erase(0, s_index + 1);
        s_index = instruction.find(",");
        part[2] = regname_to_binary(instruction.substr(0, s_index)); // stores destination register in 5 bit binary format
        instruction.erase(0, s_index + 2);
        s_index = instruction.find(",");
        part[3] = regname_to_binary(instruction.substr(0, s_index)); // stores source register 1 in 5 bit binary format
        instruction.erase(0, s_index + 2);
        s_index = instruction.find(",");
        part[4] = regname_to_binary(instruction.substr(0, s_index)); // stores source register 2 in 5 bit binary format

        return part;
    }
    else if (part[1] == "addi" || part[1] == "xori" || part[1] == "ori" || part[1] == "andi" || part[1] == "slli" || part[1] == "srli" || part[1] == "srai")
    {
        if (std::count(instruction.begin(), instruction.end(), ',') != 2)
        {
            part[0] = "Error: Instruction expects 3 arguments ";
            return part;
        }
        if (std::count(instruction.begin(), instruction.end(), ' ') != 3)
        {
            part[0] = "Error: Incorrect Format ";
            return part;
        }
        instruction.erase(0, s_index + 1);
        s_index = instruction.find(",");
        part[2] = regname_to_binary(instruction.substr(0, s_index)); // stores destination register in binary format
        instruction.erase(0, s_index + 2);
        s_index = instruction.find(",");
        part[3] = regname_to_binary(instruction.substr(0, s_index)); // stores source register in binary form
        instruction.erase(0, s_index + 2);
        s_index = instruction.find(",");
        part[5] = instruction.substr(0, s_index); // stores immediate value in string
        return part;
    }
    else if (part[1] == "lb" || part[1] == "lh" || part[1] == "lw" || part[1] == "ld" || part[1] == "lbu" || part[1] == "lhu" || part[1] == "lwu" || part[1] == "jalr")
    {
        if (std::count(instruction.begin(), instruction.end(), ',') != 1)
        {
            part[0] = "Error: Instruction expects 3 arguments ";
            return part;
        }
        if (std::count(instruction.begin(), instruction.end(), ' ') != 2)
        {
            part[0] = "Error: Incorrect Format ";
            return part;
        }
        instruction.erase(0, s_index + 1);
        s_index = instruction.find(",");
        part[2] = regname_to_binary(instruction.substr(0, s_index)); // stores destination register in binary format
        instruction.erase(0, s_index + 2);
        s_index = instruction.find("(");
        if (s_index == string::npos)
        {
            part[0] = "Error: Invalid Format";
            return part;
        }
        part[5] = instruction.substr(0, s_index); // stores immediate value in string
        instruction.erase(0, s_index + 1);
        s_index = instruction.find(")");
        if (s_index == string::npos)
        {
            part[0] = "Error: Invalid Format";
            return part;
        }
        part[3] = regname_to_binary(instruction.substr(0, s_index)); // stores source register in binary form
        return part;
    }
    else if (part[1] == "sb" || part[1] == "sh" || part[1] == "sw" || part[1] == "sd")
    {
        if (std::count(instruction.begin(), instruction.end(), ',') != 1)
        {
            part[0] = "Error: Instruction expects 3 arguments ";
            return part;
        }
        //   returns error string if space is not present b/w comma and next argument
        if (std::count(instruction.begin(), instruction.end(), ' ') != 2)
        {
            part[0] = "Error: Incorrect Format ";
            return part;
        }
        instruction.erase(0, s_index + 1);
        s_index = instruction.find(",");
        part[4] = regname_to_binary(instruction.substr(0, s_index)); // stores second source register in 5-bit binary format
        instruction.erase(0, s_index + 2);
        s_index = instruction.find("(");
        if (s_index == string::npos)
        {
            part[0] = "Error: Invalid Format";
            return part;
        }
        part[5] = instruction.substr(0, s_index); // stores immediate value in string
        instruction.erase(0, s_index + 1);
        s_index = instruction.find(")");
        if (s_index == string::npos)
        {
            part[0] = "Error: Invalid Format";
            return part;
        }
        part[3] = regname_to_binary(instruction.substr(0, s_index)); // stores first source register in 5-bit binary format
        return part;
    }
    else if (part[1] == "beq" || part[1] == "bne" || part[1] == "blt" || part[1] == "bge" || part[1] == "bltu" || part[1] == "bgeu")
    {
        // when calling from  B_format fn, it has neat representation with 'L1:' removed and label name changed to offset integer string
        instruction.erase(0, s_index + 1);
        s_index = instruction.find(",");
        part[3] = regname_to_binary(instruction.substr(0, s_index)); // stores first source register in binary format
        // if any register name is not a valid register name, it returns the error message that regname_to_binary function gave
        instruction.erase(0, s_index + 2);
        s_index = instruction.find(",");
        part[4] = regname_to_binary(instruction.substr(0, s_index)); // stores second source register in binary format
        instruction.erase(0, s_index + 2);
        s_index = instruction.find(",");
        part[5] = instruction.substr(0, s_index); // stores immediate value in string
        return part;
    }
    else if (part[1] == "jal")
    {
        instruction.erase(0, s_index + 1);
        s_index = instruction.find(",");
        part[2] = regname_to_binary(instruction.substr(0, s_index)); // stores destination register in binary format
        instruction.erase(0, s_index + 2);
        s_index = instruction.find(" ");
        part[5] = instruction.substr(0, s_index); // stores immediate value in string
        return part;
    }
    else if (part[1] == "lui")
    {
        instruction.erase(0, s_index + 1);
        s_index = instruction.find(",");
        part[2] = regname_to_binary(instruction.substr(0, s_index)); // stores destination register in binary format
        // if any register name is not a valid register name, it returns the error message that regname_to_binary function gave
        instruction.erase(0, s_index + 2);
        s_index = instruction.find(" ");
        part[5] = instruction.substr(0, s_index); // stores immediate value in string
        return part;
    }
    else
    {
        part[0] = "Error: Invalid instruction name ";
        return part;
    }
}