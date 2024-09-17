#include <iostream>
#include <fstream>
#include <string>
#include <algorithm> // for reversing string during deci_to_bin conversion
#include <cmath>     // for 'pow' function
#include <vector>
using namespace std;
#include "assembler.h"

int main()
{
    vector<string> ins_lines; // stores each line in vector of string type(to know label defns)
    string word, s_offset, ins_name;
    int c = 0, i = 0, t = 0, offset, m = 0, j = 0;
    struct label_info
    { // stores the label name and in which line it was defined
        string name;
        int line_no;
    };
    vector<label_info> label; // stores info of diff labels

    ifstream in("input.s");     // opens the file to read
    string instruction;         // to store one instruction
    ofstream out("output.hex"); // opens the file to write or creates one if not present
    string hexcode;             // stores final hexcode

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
            cout << "Error in Line " << c + 1 << " :" << hexcode.substr(6) << endl;
        }
    }

    in.close();  // closing input file
    out.close(); // closing output file
    return 0;
}
