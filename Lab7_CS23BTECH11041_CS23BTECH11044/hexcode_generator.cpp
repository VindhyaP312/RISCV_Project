#include <iostream>
#include <fstream>
#include <string>
#include <algorithm> // for reversing string during deci_to_bin conversion //for transforming data to lower case
#include <cmath>     // for 'pow' function
#include <vector>
#include <limits>
using namespace std;
#include "riscv_header.h"

vector<string> ins_lines; // stores each line in vector of string type(to know label defns)
vector<label_info> label; // stores info of diff labels
int data_ptr = 0;   // is used to initialize .data section in data_mem map
int data_line_count = 0;    // line counter of data section

// size specifies no. of hexa decimal characters to be stored in memory
void store_in_memory(string input, int size)
{
    while (!input.empty())
    {
        // find the number till reaching comma and convert to hex bits
        int findComma = input.find(',');
        string number;
        if( findComma != string :: npos){
            number = input.substr(0,findComma);
            input.erase(0,findComma+2);
        }
        else{
            number = input;
            input.clear();
        }
        string hex_input;

        // if has 0x or 0X at start
        if (number[0] == '0' && (number[1] == 'x' || number[1] == 'X'))
        {
            hex_input = number;
            hex_input.erase(0, 2); // stores hexadecimal number(after removing first 2 chars i.e, 0 and x) 
            int length = hex_input.length();
            if(length > size){
                cout<<"Error: data exceeds size limit."<<endl;
                load_error = 1;
                return;
            }
            else
                hex_input.insert(hex_input.begin(),size-length,'0');
        }
        else if(all_of(number.begin(), number.end(), ::isdigit) || number[0]=='-' && all_of(number.begin() + 1, number.end(), ::isdigit))
        {
            // converts decimal number to hexa decimal of specified size if given is decimal input  - 234
            if(size == 16){                
                long long signed_value;
                try {
                    if (number[0]=='-') {
                        // Convert the number using stoll for negative values
                        signed_value = stoll(number);

                        // Ensure the value does not exceed the long long min value
                        if (signed_value < std::numeric_limits<long long>::min()) {
                            cout << "Error in Line "<<data_line_count<<" : data exceeds size limit." << endl;
                            load_error =1;
                            return;
                        }
                    } 
                    else {
                        // Convert the number using stoul for non-negative values
                        unsigned long long deci_val = stoul(number);
                        
                        // Check if the value exceeds the signed long long max value
                        if (deci_val > static_cast<unsigned long long>(std::numeric_limits<long long>::max())) {
                            cout << "Error in Line "<<data_line_count<<" : data exceeds size limit." << endl;
                            load_error = 1;
                            return;
                        }
                        
                        signed_value = static_cast<long long>(deci_val);
                        
                    }
                    hex_input = deci_to_hex(number, size);
                } catch (const std::out_of_range& e) {
                    cout << "Error in Line "<<data_line_count<<" : data exceeds size limit." << endl;
                    load_error =1;
                    return;
                } catch (const std::invalid_argument& e) {
                    cout << "Error: invalid number format." << endl;
                    load_error =1;
                    return;
                }
            }
            else{
                long int max_value = (1 << (size*4 - 1)) - 1; // For positive numbers
                long int min_value = -(1 << (size*4 - 1));   // For negative numbers
                if(number.length() > 11){
                    cout << "Error in Line "<<data_line_count<<" : data exceeds size limit." << endl;
                    load_error =1;
                    return;
                }
                if (stol(number) > max_value || stol(number) < min_value)
                { // Checking if the number is within bounds
                    cout << "Error in Line "<<data_line_count<<" : data exceeds size limit." << endl;
                    load_error =1;
                    return;
                }
                hex_input = deci_to_hex(number, size);
            }
        }
        else{
            cout<< "Error: Invalid format" << endl;
            continue;
        }
        if(load_error != 1){
            transform(hex_input.begin(), hex_input.end(), hex_input.begin(), ::tolower);
            for (int i = size - 1; i >= 0; i--)
                {
                    // push these hex digits to data section
                    data_mem[data_ptr] = hex_input[i];
                    data_ptr++;
                }
        }
    }
}

void machine_code(string file_name)
{
    string word, s_offset, ins_name;
    int c = 0, i = 0, t = 0, offset, m = 0, j = 0;
    data_line_count = 0;

    ifstream in(file_name);     // opens the file to read
    // if any error in opening input file
     if (!in) {
        cout << "Error opening file!" << endl;
        load_error = 1;
        return ;
    }
    string instruction;         // to store one instruction
    ofstream out("output.hex"); // opens the file to write or creates one if not present
    // if any error in opening output file
    if (!out) {
        cout << "Error opening file!" << endl;
        load_error = 1;
        return ;
    }
    string hexcode;             // stores final hexcode

    // clearing all memory
    // suppose we are calling load for second time, ensuring that these segments regs,text and data are clear
    initialize_reg_values(); // regs to default values
    text_mem.clear();
    data_mem.clear();
    label.clear();
    ins_lines.clear();
    lineNo = 0;
    data_ptr = 0;
    break_points.clear();
    call_stack.clear();
    call_stack.push_back({"main",0});

    
    getline(in, instruction);
    data_line_count ++;
    // checks for .data section
    if (instruction.find(".data") != string::npos)
    {
        getline(in, instruction);
        data_line_count ++;
        // runs loop till it finds .text section
        while (instruction.find(".text") == string::npos)
        {
            if (instruction.find(".dword") != string::npos)
            {
                // erase the ".dword "
                instruction.erase(0, 6);

                // if in diff line values are provided
                if (instruction.empty())
                {
                    getline(in, instruction);
                    data_line_count ++;

                }
                // if in same line values are provided but with space after .dword
                else
                {
                    instruction.erase(0, 1); // removes space if in sam eline
                }

                store_in_memory(instruction, 16);
            }
            else if (instruction.find(".word") != string::npos)
            {
                instruction.erase(0, 5);
                // if diff line
                if (instruction.empty())
                {
                    getline(in, instruction);
                    data_line_count ++;

                }
                else
                {
                    instruction.erase(0, 1); // removes space if in same line
                }
                store_in_memory(instruction, 8);
            }
            else if (instruction.find(".half") != string::npos)
            {
                instruction.erase(0, 5);
                // if diff line
                if (instruction.empty())
                {
                    getline(in, instruction);
                    data_line_count ++;

                }
                else
                {
                    instruction.erase(0, 1); // removes space if in same line
                }
                store_in_memory(instruction, 4);
            }
            else if (instruction.find(".byte") != string::npos)
            {
                instruction.erase(0, 5);
                // if diff line
                if (instruction.empty())
                {
                    getline(in, instruction);
                    data_line_count ++;

                }
                else
                {
                    instruction.erase(0, 1); // removes space if in same line
                }
                store_in_memory(instruction, 2);
            }
                // Incorrect data type
            else{
                cout << "Error in Line "<< data_line_count <<" : Incorrect datatype." << endl;
                load_error =1;
                return;
            }
            getline(in, instruction);
            data_line_count ++;

        }
    }
    // if .text is present in first line
    else if(instruction.find(".text") != string::npos){}
    // if directly instruction is present in first line
    else{
        in.clear(); // Clear any EOF or error flags
        in.seekg(0); // Move pointer to the start of the file
        data_line_count = 0;
    }

    while (getline(in, instruction))
    {
        ins_lines.push_back(instruction); // stores instructions
        // If colon is present, it stores the label_name and line no. in vector 'label' which contains 'label_info' as elements
        if (instruction.find(':') != std::string::npos)
        { // if found ':' in that line
            label.push_back({instruction.substr(0, instruction.find(':')), c});
        }
        c++; // incrementing the line number, to store the next instruction in vector
    }

    for (c = 0; c < ins_lines.size(); c++)
    {
        j = 0;                      // to check for multiple label defns
        instruction = ins_lines[c]; // stores that line in 'instruction' variable
        if (instruction.find(':') != std::string::npos)
        { // if label is defined in this line
            word = instruction.substr(0, instruction.find(':'));
            for (i = 0; i < label.size(); i++)
            {
                if (label[i].name == word)
                { // searching for the first occurence of that label from vector 'label'
                    break;
                }
            }
            if (i < label.size())
            {
                // checking again if any label with same name is defined in any other line other than line no. 'i'
                for (m = 0; m < label.size(); m++)
                {
                    if (label[m].name == word && m != i)
                    {        // if label is defined multiple times, stores hexcode as this
                        j++; // increments 'j' if multiple lael defns are present
                        hexcode = "Error: Multiple Definitions of Label: " + word;
                        break;
                    }
                }
            }
            // if label is defined only once, removes the label name from that line and uses
            if (j == 0)
                instruction.erase(0, instruction.find(':') + 2);
        }

        ins_name = instruction.substr(0, instruction.find(' ')); // read the name i.e, first word

        char formatType = findformat(ins_name); // returns format type of that instruction
        // switch cases for diff types of formats
        if (j == 0)
        { // if label is not present or if label is defined only once
            switch (formatType)
            {
            case 'R':
                /*
                 * calls R_format fn passing the whole instruction
                 * It returns either the 8 digit hexcode or any error if smtg is wrong in instruction line
                 * Same for I, S and U format types
                 */
                hexcode = R_format(instruction);
                break;
            case 'I':
                hexcode = I_format(instruction);
                break;
            case 'S':
                hexcode = S_format(instruction);
                break;
            case 'B':
                // branch has 3 arguments: 2 registers and one label: so 2 commas should be present
                if (std::count(instruction.begin(), instruction.end(), ',') != 2)
                {
                    hexcode = "Error: Instruction expects 3 arguments "; // if 2 commas are not present
                    break;
                }
                // below 'if' statement ensures that, there are spaces b/w commas and arguments
                if (std::count(instruction.begin(), instruction.end(), ' ') != 3)
                {
                    hexcode = "Error: Incorrect Format ";
                    break;
                }

                t = instruction.find_last_of(' ');
                // 'word' stores the 3rd argument of branch instruction
                word = instruction.substr(t + 1, instruction.length() - t - 1);
                // directly calls the B_format fn if 3rd argument is +ve or -ve decimal or hexadecimal number
                if (all_of(word.begin(), word.end(), ::isdigit) || (word[0] == '-' && all_of(word.begin() + 1, word.end(), ::isdigit)) || word[0] == '0' && (word[1] == 'x' || word[1] == 'X') || word[0] == '-' && word[1] == '0' && (word[2] == 'x' || word[2] == 'X'))
                {
                    hexcode = B_format(instruction);
                    break;
                }
                // if 3rd argument is label name, it converts that to decimal offset and then calls B_format fn
                else
                {
                    for (i = 0; i < label.size(); i++)
                    { // if the 3rd argument is present as label
                        if (label[i].name == word)
                        {
                            break;
                        }
                    }
                    if (i < label.size())
                    {
                        for (m = i + 1; m < label.size(); m++)
                        { // if label definition is present multiple times
                            if (label[m].name == word)
                            {
                                hexcode = "Error: Multiple Definitions of Label: " + word;
                                break; // breaks from 'for' loop
                            }
                        }
                        if (m != label.size()) // if label definition is present multiple times, breaks from this switch case
                            break;
                        instruction.erase(t + 1, instruction.length() - t - 1); // erases the label name
                        offset = (label[i].line_no - c) * 4;                    // calculates the offset
                        s_offset = to_string(offset);                           // convets to string form
                        instruction += s_offset;                                // append to instruction
                        hexcode = B_format(instruction);                        // calls B_format fn
                        break;
                    }
                    hexcode = "Error: Label does not exist"; // if no label is defined with that 3rd argument
                    break;
                }

            case 'J':
                // jump has 2 arguments: 1 register and 1 label: so 1 comma should be present
                if (std::count(instruction.begin(), instruction.end(), ',') != 1)
                {
                    hexcode = "Error: Instruction expects 2 arguments ";
                    break;
                }
                // below 'if' statement ensures that, there are spaces b/w commas and arguments
                if (std::count(instruction.begin(), instruction.end(), ' ') != 2)
                {
                    hexcode = "Error: Incorrect Format ";
                    break;
                }
                t = instruction.find_last_of(' ');
                // 'word' stores the 2nd argument of jump instruction
                word = instruction.substr(t + 1, instruction.length() - t - 1);
                // directly calls the J_format fn if 2nd argument is +ve or -ve decimal or hexadecimal number
                if (all_of(word.begin(), word.end(), ::isdigit) || (word[0] == '-' && all_of(word.begin() + 1, word.end(), ::isdigit)) || word[0] == '0' && (word[1] == 'x' || word[1] == 'X') || word[0] == '-' && word[1] == '0' && (word[2] == 'x' || word[2] == 'X'))
                {
                    hexcode = J_format(instruction);
                    break;
                }
                // if 2nd argument is label name, it converts that to decimal offset and then calls J_format fn
                else
                {
                    for (i = 0; i < label.size(); i++)
                    { // if the 2nd argument is present as label
                        if (label[i].name == word)
                        {
                            break;
                        }
                    }
                    if (i < label.size())
                    {
                        for (m = i + 1; m < label.size(); m++)
                        { // if label definition is present multiple times
                            if (label[m].name == word)
                            {
                                hexcode = "Error: Multiple Definitions of Label: " + word;
                                break; // breaks from 'for' loop
                            }
                        }
                        if (m != label.size())
                            break;
                        instruction.erase(t + 1, instruction.length() - t - 1); // erases the label name
                        offset = (label[i].line_no - c) * 4;                    // calculates the offset
                        s_offset = to_string(offset);                           // convets to string form
                        instruction += s_offset;                                // append to instruction
                        hexcode = J_format(instruction);                        // calls J_format fn
                        break;
                    }
                    hexcode = "Error: Label does not exist"; // if no label is defined with that 3rd argument
                    break;
                }
            case 'U':
                hexcode = U_format(instruction);
                break;
            default: // if no format type matches
                if (instruction.empty())
                {
                    hexcode = "Error: Blank line";
                }
                else
                {
                    hexcode = "Error: Incorrect Instruction Name";
                }
                break;
            }
        }
        // In 'output.hex' file,  it either give correct 8 bit hex code or any error if occured due to mistakes in instruction line
        out << hexcode << endl;
        // In terminal, it provides the line number in which error occured along with specifying the error
        if (hexcode.find("Error") != string::npos)
        {
            cout << "Error in Line " << c + 1 + data_line_count << " :" << hexcode.substr(6) << endl;
        }
    }

    in.close();  // closing input file
    out.close(); // closing output file
    return;
}
