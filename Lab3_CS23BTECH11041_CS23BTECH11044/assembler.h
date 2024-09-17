#include <iostream>
#include <fstream>
#include <string>
#include <algorithm> // for reversing string during deci_to_bin conversion
#include <cmath>     // for 'pow' function
#include <vector>
using namespace std;

char findformat(string);
string bin_to_hex(string );
string deci_to_bin(string, int );
string regname_to_binary(string );
string immediate_binary(string, int );
string bin_to_hex(string);
string R_format(string);
string I_format(string);
string S_format(string);
string B_format(string);
string J_format(string);
string U_format(string);
