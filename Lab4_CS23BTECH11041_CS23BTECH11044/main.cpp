#include <iostream>
#include <fstream>
#include <string>
#include <algorithm> // for reversing string during deci_to_bin conversion
#include <cmath>     // for 'pow' function
#include <vector>
#include <map> // to store registers and its value
using namespace std;
#include "riscv_header.h"

int load_error;
int lineNo; // starts from 0 to ins_lines.size()-1
string PC;  // to print PC value
map<int, char> text_mem;    // maps to text memory
map<int,char> data_mem; // maps to data memory
// map from binary 5 bits to the long int(64bits storage capacity)
map<string, long int> reg_value;
map<int,bool> break_points; // map to line number where breakpoints are present
vector<fn_call> call_stack;     // stores fn calls with its fn name and line no.


int main()
{
    load_error = -1; //load_error = -1 : no file loaded, = 1: load file has errors, = -2: incorrect memory access, = -3 incorrect instruction address
    string input;
    // runs a infinte loop
    while (true)
    {   
        // reads each line
        getline(cin, input);

        // if load
        if (input.find("load") != string::npos)
        {
            load_error = 0;
            // get filename from 2nd part of input string
            string file_name = input.substr(input.find(' ') + 1);
            // generates machine code
            machine_code(file_name);
            ifstream in("output.hex");
            string line;
            while(getline(in, line)){
                if(line.find("Error") != string::npos){
                    load_error = 1;
                    break;
                }
            }
            if(load_error == 1){
                cout<<"Input file has error. Please load another file."<<endl<<endl;
                continue;
            }
            // initializes text memory with hex codes of each instruction
            initializeTextMem();
            cout<<endl;
        }
        else if (input.find("run") != string::npos)
        {
            // below checks are for not to run before correcting any errors in input file if present
            if(load_error == 1){
                cout<<"Input file has error. Please load another file."<<endl<<endl;
                continue;
            }
            else if(load_error == -1){
                cout<<"No file has been loaded. Please load a file."<<endl<<endl;
                continue;
            }
            else if(load_error == -2){
                cout<<"Error: Code is writing to read-only memory. Please load another file."<<endl<<endl;
                continue;
            }
            else if(load_error == -3){
                cout<<"Error: Code is accessing invalid instruction address. Please load another file."<<endl<<endl;
                continue;
            }
            if(lineNo == ins_lines.size()){
                cout << "Nothing to run" << endl << endl;
            }
            else{
                run();
            }
            
        }
        else if (input.find("regs") != string::npos)
        {
            // below checks are for not to use regs command before correcting any errors in input file if present

            if(load_error == 1){
                cout<<"Input file has error. Please load another file."<<endl<<endl;
                continue;
            }
            else if(load_error == -1){
                cout<<"No file has been loaded. Please load a file."<<endl<<endl;
                continue;
            }
            else if(load_error == -2){
                cout<<"Error: Code is writing to read-only memory. Please load another file."<<endl<<endl;
                continue;
            }
            else if(load_error == -3){
                cout<<"Error: Code is accessing invalid instruction address. Please load another file."<<endl<<endl;
                continue;
            }
            // prints register values
            print_reg_values();
        }
        else if (input.find("exit") != string::npos)
        {
            // exits simulator
            // clears all the memory
            cout << "Exited the simulator" << endl;
            text_mem.clear();
            data_mem.clear();
            label.clear();
            ins_lines.clear();
            break_points.clear();
            call_stack.clear();
            return 0;
        }
        else if (input.find("mem") != string::npos)
        {
            // below checks are for not to use mem command before correcting any errors in input file if present

            if(load_error == 1){
                cout<<"Input file has error. Please load another file."<<endl<<endl;
                continue;
            }
            else if(load_error == -1){
                cout<<"No file has been loaded. Please load a file."<<endl<<endl;
                continue;
            }
            else if(load_error == -2){
                cout<<"Error: Code is writing to read-only memory. Please load another file."<<endl<<endl;
                continue;
            }
            else if(load_error == -3){
                cout<<"Error: Code is accessing invalid instruction address. Please load another file."<<endl<<endl;
                continue;
            }
            // erasing "mem "
            input.erase(0, input.find(' ') + 1);
            // gets address
            string addr = input.substr(0, input.find(' '));
            input.erase(0, input.find(' ') + 1);
            // gets count
            int count = stoi(input);
            // calls fn to print memory values
            print_memory_values(addr, count);
        }
        else if (input.find("step") != string::npos)
        {
            // below checks are for not to use step command before correcting any errors in input file if present

            if(load_error == 1){
                cout<<"Input file has error. Please load another file."<<endl<<endl;
                continue;
            }
            else if(load_error == -1){
                cout<<"No file has been loaded. Please load a file."<<endl<<endl;
                continue;
            }
            else if(load_error == -2){
                cout<<"Error: Code is writing to read-only memory. Please load another file."<<endl<<endl;
                continue;
            }
            else if(load_error == -3){
                cout<<"Error: Code is accessing invalid instruction address. Please load another file."<<endl<<endl;
                continue;
            }
            step();
            cout << endl;
        }
        else if(input.find("show-stack") != string::npos){
            // below checks are for not to use show-stack command before correcting any errors in input file if present

            if(load_error == 1){
                cout<<"Input file has error. Please load another file."<<endl<<endl;
                continue;
            }
            else if(load_error == -1){
                cout<<"No file has been loaded. Please load a file."<<endl<<endl;
                continue;
            }
            else if(load_error == -2){
                cout<<"Error: Code is writing to read-only memory. Please load another file."<<endl<<endl;
                continue;
            }
            else if(load_error == -3){
                cout<<"Error: Code is accessing invalid instruction address. Please load another file."<<endl<<endl;
                continue;
            }
            print_call_stack();
            cout << endl;
        }
        else if (input.find("del break") != string::npos)
        {
            // below checks are for not to use del-break command before correcting any errors in input file if present

            if(load_error == 1){
                cout<<"Input file has error. Please load another file."<<endl<<endl;
                continue;
            } 
            else if(load_error == -1){
                cout<<"No file has been loaded. Please load a file."<<endl<<endl;
                continue;
            } 
            else if(load_error == -2){
                cout<<"Error: Code is writing to read-only memory. Please load another file."<<endl<<endl;
                continue;
            }  
            else if(load_error == -3){
                cout<<"Error: Code is accessing invalid instruction address. Please load another file."<<endl<<endl;
                continue;
            }        
            // erasing "del break "
            input.erase(0,10);
            auto it = break_points.find(stoi(input)-1 - data_line_count);
            if(it != break_points.end()){
                break_points.erase(it);
                cout << "Breakpoint deleted at line " << input;
            }
            else{
                cout << "Breakpoint at line " << input <<" is not set";
            }
            cout << endl << endl;
        }
        else if (input.find("break") != string::npos)
        {
            // below checks are for not to use break command before correcting any errors in input file if present

            if(load_error == 1){
                cout<<"Input file has error. Please load another file."<<endl<<endl;
                continue;
            }
            else if(load_error == -1){
                cout<<"No file has been loaded. Please load a file."<<endl<<endl;
                continue;
            }
            else if(load_error == -2){
                cout<<"Error: Code is writing to read-only memory. Please load another file."<<endl<<endl;
                continue;
            }
            else if(load_error == -3){
                cout<<"Error: Code is accessing invalid instruction address. Please load another file."<<endl<<endl;
                continue;
            }
            // erasing "break "
            input.erase(0,6);
            if(stoi(input) > ins_lines.size() + data_line_count){
                cout << "Breakpoint cannot be set. Line exceeds the last line." << endl << endl;
            }
            else{
            // stores that line number
            break_points[stoi(input)-1 - data_line_count] = 0;
            cout << "Breakpoint set at line " << input << endl << endl;
            }
        }
        else{
            cout << "Invalid request" << endl<<endl;
        }
    }
    return 0;
}

