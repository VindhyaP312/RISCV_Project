#include <iostream>
#include <fstream>
#include <string>
#include <algorithm> // for reversing string during deci_to_bin conversion
#include <cmath>     // for 'pow' function
#include <vector>
#include <map>      // to store registers and its value
#include <limits>
using namespace std;

extern bool isCacheEnabled;
struct cache_info{
    int cacheSize;
    int blockSize;
    int associativity;
    string replacementPolicy;
    string writeBackPolicy;
    int rows;
};
extern cache_info configDetails;
struct cacheStats{
    int access;
    int hit;
    int miss;
};
extern cacheStats stats;
struct blockDetails{
    bool isValid;
    bool isDirty;
    int tag;
    string data; // stores hex chars as data
    int rpOrder; // variable to store replacement ordering
};
extern map<int,vector<blockDetails>> cache;
extern string cache_output_filename;

vector<int> parse_address(long int);
string getDataFromMemory(long int);
int find_min_rpOrder(int);
string getDataFromCache(long int, int);
void storeDataInMemory(long int, int, string);
void inValidate_cache();
void print_Cache_Statistics();
void updateMemory(long int, int, string);
void updateCache(int, int, int, int, string);
bool isPower_of_2(int);

struct label_info
{ // stores the label name and in which line it was defined
    string name;
    int line_no;
};
extern vector<string> ins_lines; // stores each line in vector of string type(to know label defns)
extern vector<label_info> label; // stores info of diff labels
extern map<int,char> text_mem;
extern map<int,char> data_mem;
extern map<string,long int> reg_value;
extern map<int,bool> break_points;
extern int lineNo;
extern int data_line_count;
extern int load_error;

struct fn_call{
    string fn_name;
    int fn_last_line;
};
extern vector<fn_call> call_stack;

void print_reg_values();
void print_memory_values(string addr, int count);
void print_call_stack();
void step();
void run();

void machine_code(string);
vector<string> parse(string);
char findformat(string);
string bin_to_hex(string );
string bin_to_hex_sz(string, int );
string deci_to_bin(string, int );   
string regname_to_binary(string );
string immediate_binary(string, int );
string bin_to_hex(string);
string deci_to_hex(string, int);
void initialize_reg_values();
void initializeTextMem();
long int hex_to_deci(string);

string R_format(string);
string I_format(string);
string S_format(string);
string B_format(string);
string J_format(string);
string U_format(string);

int R_execute(vector<string>);
int I_execute(vector<string>);
int S_execute(vector<string>);
int B_execute(vector<string>);
int J_execute(vector<string>);
int U_execute(vector<string>);
