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

bool isCacheEnabled;
map<int,vector<blockDetails>> cache;
cache_info configDetails;
cacheStats stats;
string cache_output_filename = "";


int main()
{
    srand(time(0));
    load_error = -1; //load_error = -1 : no file loaded, = 1: load file has errors, = -2: incorrect memory access, = -3 incorrect instruction address, = -4: multiple block access, 2 : current file finished, new file to be run
    string input;
    // runs a infinte loop
    while (true)
    {   
        // reads each line
        getline(cin, input);

        if(input.find("cache_sim enable") != string::npos){ 
            if(load_error == 0){
                cout<<"Cannot enable/change cache configs after loading the input file."<<endl<<endl;
                continue;
            }
            // gets the config file name 
            string file_name = input.substr(input.find_last_of(' ') + 1);
            isCacheEnabled = 1;
            ifstream in(file_name);     // opens the file to read
            // if any error in opening input file
            if (!in) {
                cout << "Error opening file!" << endl;
            }
            in >> configDetails.cacheSize >> configDetails.blockSize >> configDetails.associativity;
            // if fully associative
            if(configDetails.associativity == 0){
                configDetails.associativity = configDetails.cacheSize/configDetails.blockSize;
            }
            in >> configDetails.replacementPolicy >> configDetails.writeBackPolicy;
            transform(configDetails.replacementPolicy.begin(), configDetails.replacementPolicy.end(), configDetails.replacementPolicy.begin(), ::toupper);
            transform(configDetails.writeBackPolicy.begin(), configDetails.writeBackPolicy.end(), configDetails.writeBackPolicy.begin(), ::toupper); //if the user does not write the options in all CAPS it is still fine, we are considering that as well to be correct
            in.close();     // closing input file

            //if config file has error, like out of range values for block size or error in replacement and write policy names, we give error that cannot enable cache, keep the cache disabled and proceed
            if(configDetails.cacheSize <=0 || configDetails.blockSize <=0 || configDetails.associativity < 0 || !isPower_of_2(configDetails.cacheSize) || !isPower_of_2(configDetails.blockSize) || !isPower_of_2(configDetails.associativity) || (configDetails.replacementPolicy != "LRU" && configDetails.replacementPolicy != "FIFO" && configDetails.replacementPolicy != "RANDOM") || (configDetails.writeBackPolicy != "WB" && configDetails.writeBackPolicy != "WT") ) {
                cout<<"Cache config file has errors, cannot enable cache"<<endl<<endl;
                isCacheEnabled = 0;
                continue;
            }

            // stores count of rows
            configDetails.rows = configDetails.cacheSize/(configDetails.blockSize*configDetails.associativity);

            // declaring keys and resizing vectors
            for(int i=0; i< configDetails.rows;i++){
                cache[i].resize(configDetails.associativity);
            }
            //initializing cache with default values
            inValidate_cache();

            // initializing access, hit, miss in stats as 0
            stats.access = 0;    
            stats.hit = 0;
            stats.miss = 0;

            cout << endl;
        }
        // if cache disable
        else if(input.find("cache_sim disable")!= string::npos){
            if(load_error == 0){
                cout<<"Cannot disable cache configs after loading the input file."<<endl<<endl;
                continue;
            }
            isCacheEnabled = 0;
            cout << endl;
        }
        // if print cache status
        else if(input.find("cache_sim status")!= string::npos){
            if(isCacheEnabled){
                cout  << "Cache is enabled" << endl;
                cout << "Cache Size: " << configDetails.cacheSize << endl;
                cout << "Block Size: " << configDetails.blockSize << endl << "Associativity: ";
                if(configDetails.associativity == configDetails.cacheSize/configDetails.blockSize)  cout << 0;
                else    cout << configDetails.associativity;
                cout << endl << "Replacement Policy: " << configDetails.replacementPolicy << endl;
                cout << "Write Back Policy: " << configDetails.writeBackPolicy << endl << endl;
            }
            else{
                cout << "Cache is disabled" << endl << endl;
            }
        }
        // if cache in validate
        else if(input.find("cache_sim invalidate") != string::npos){
            //initializing cache with default values
            if(isCacheEnabled){
                inValidate_cache();
                cout << endl;
            }
            else cout<<"Cache is disabled"<<endl<<endl;
        }
        else if(input.find("cache_sim dump") != string::npos){
            if(isCacheEnabled){
                // gets the config file name 
                string file_name = input.substr(input.find_last_of(' ') + 1);
                ofstream out(file_name);     // opens the file to write
                // if any error in opening this file
                if (!out) {
                    cout << "Error opening file!" << endl;
                }
                // writing all valid entries from cache map to this file
                for(int i=0; i< configDetails.rows;i++){
                    for(int j=0; j<configDetails.associativity;j++){
                        // only if that block is valid
                        if(cache[i][j].isValid){
                            out << "Set: 0x" << std::hex << i << ", Tag: 0x" << cache[i][j].tag << ", ";
                            if(cache[i][j].isDirty)     out << "Dirty" << endl;
                            else    out << "Clean" << endl;
                        }
                    }
                }
                out.close();
                cout << endl;
            }
            else cout<<"Cache is disabled"<<endl<<endl;
        }
        else if(input.find("cache_sim stats") != string::npos){
            if(isCacheEnabled){
                print_Cache_Statistics();
                cout << endl;
            }
            else cout<<"Cache is disabled"<<endl<<endl;
        }

        // if load
        else if (input.find("load") != string::npos)
        {
            load_error = 0;
            // get filename from 2nd part of input string
            string file_name = input.substr(input.find(' ') + 1);
            if(isCacheEnabled) {
                //initializing cache with default values
                inValidate_cache();
                // initializing access, hit, miss in stats as 0
                stats.access = 0;    
                stats.hit = 0;
                stats.miss = 0;
                cache_output_filename = file_name.substr(0,file_name.find('.')) + ".output"; 
                // clearing the output file before using it to write
                ofstream out(cache_output_filename,std::ios::trunc);
                if(out.is_open()){
                    out.close();
                }
            }
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
            else if(load_error == -4){
                cout<<"Error: Code is accessing multiple blocks in cache. Please load another file."<<endl<<endl;
                continue;
            }
            if(lineNo == ins_lines.size()){
                load_error = 2;
                cout << "Nothing to run" << endl << endl;
            }
            else{
                run(); //*****************************************************what about breakpoints, wont this print after each run in the breakpoint? */
                if(isCacheEnabled)      print_Cache_Statistics();
                cout << endl;
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
            else if(load_error == -4){
                cout<<"Error: Code is accessing multiple blocks in cache. Please load another file."<<endl<<endl;
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
            else if(load_error == -4){
                cout<<"Error: Code is accessing multiple blocks in cache. Please load another file."<<endl<<endl;
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
            else if(load_error == -4){
                cout<<"Error: Code is accessing multiple blocks in cache. Please load another file."<<endl<<endl;
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
            else if(load_error == -4){
                cout<<"Error: Code is accessing multiple blocks in cache. Please load another file."<<endl<<endl;
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
            else if(load_error == -4){
                cout<<"Error: Code is accessing multiple blocks in cache. Please load another file."<<endl<<endl;
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
            else if(load_error == -4){
                cout<<"Error: Code is accessing multiple blocks in cache. Please load another file."<<endl<<endl;
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