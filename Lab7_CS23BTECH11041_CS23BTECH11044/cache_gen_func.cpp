#include <iostream>
#include <iomanip> // for setprecision and fixed
#include "riscv_header.h"
using namespace std;

bool isPower_of_2(int n){
    if((n == 0) || ((n & (n-1)) == 0)) return true;
    else return false;
}

void inValidate_cache(){
    for(int i=0; i< configDetails.rows;i++){
        for(int j=0; j<configDetails.associativity;j++){
            if(cache[i][j].isValid == true && cache[i][j].isDirty == true){
                long int addr = ((cache[i][j].tag)*(configDetails.rows) + i) * configDetails.blockSize;
                updateMemory(addr, configDetails.blockSize, cache[i][j].data);
            }
            cache[i][j].isValid = false;    // valid = 0
            cache[i][j].isDirty = false;    // dirty = 0 i.e., clean block 
            cache[i][j].tag = -1;
            cache[i][j].data = "";  // empty string
            cache[i][j].rpOrder = 0; //initializing rp with a starting default value i.e. 0
        }
    }
}

void print_Cache_Statistics(){
    cout << "D-cache statistics: Accesses=" << stats.access 
        << ", Hit=" << stats.hit 
        << ", Miss=" << stats.miss;
    cout<<std::fixed << std::setprecision(2);
    if(stats.access != 0) cout << ", Hit Rate=" << (float)(stats.hit) / stats.access << endl;
    else cout << ", Hit Rate=0"<< endl;
} 

// takes the string data of length 2*size i.e, it contains hexchars. It is stored in memory
void updateMemory(long int addr, int size, string data){
    long int rel_addr = addr - stol("0x10000",nullptr,16);
    if(rel_addr < 0){
        load_error = -2;
        return;
    }
    for (int i = 2*size-1; i >= 0; i--)
    {
        data_mem[2*rel_addr+2*size-1-i] = data[i];
    }
} 

// takes the location of block where cache data value has to be updated with new data
void updateCache(int indexValue, int blockNo, int byteOffsetValue, int size, string data){
    int i = byteOffsetValue, j=2*size - 1;
    while(size!=0){
        cache[indexValue][blockNo].data[2*configDetails.blockSize -1 - 2*i] = data[j];
        cache[indexValue][blockNo].data[2*configDetails.blockSize -1 - 2*i-1] = data[j-1];
        j = j-2;
        i++;
        size--;
    }
}

// parses given address and returns tag, index number and byte offset in a vector of <int> type
vector<int> parse_address(long int addr){
    vector<int> parsed_addr(3,0);
    int byte_offset = 
    parsed_addr[2] = addr % configDetails.blockSize;        // byte offset
    parsed_addr[1] = (addr / configDetails.blockSize) % configDetails.rows;     // index umber
    parsed_addr[0] = (addr / configDetails.blockSize) / configDetails.rows;     // tag value
    return parsed_addr;
}

string getDataFromMemory(long int addr){
    // getting data from memory
    int rel_addr = ((addr/configDetails.blockSize)*configDetails.blockSize) - stoi("0x10000",nullptr,16);
    string mem_value_hex = "";      // stores the data value to be stored in the cache block
    for(int i=0;i<configDetails.blockSize;i++){
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
    return mem_value_hex;
}

// compares all rpOrder values of each block in that row and returns the block number which has least rpOrderValue
int find_min_rpOrder(int indexNumber){
    int min = cache[indexNumber][0].rpOrder;
    int i;  
    for(i = 0; i < configDetails.associativity; i++){
        if(cache[indexNumber][i].rpOrder < min){
            min = cache[indexNumber][i].rpOrder;
        }
    }
    for(i = 0; i < configDetails.associativity; i++){
        if(cache[indexNumber][i].rpOrder == min){
            break;
        }
    }
    return i;
}

/* gets data from cache from the address specified and the data size is size mentioned in argument
- It parses the given address to tag, index and byte offset
- Finds if its a cache hit or miss:
    - if hit, we find the block no. 
    - if its a miss, we find the block number of any invalid block or will replace any block based on replacement policy. While replacing, if the block is dirty, we will update its data in memory and make it clean. Now, when replacing it gets data from memory in case of a miss and that block no. is used.
- Once we get block no., we will find the data using byte offset, index and block no.. This data will be returned.
- Access, hit and miss variables are updated accordingly
- This access details are written to output file.
*/
string getDataFromCache(long int addr, int size){
    bool isHit = false;
    // since it is accessing memory in load instructions
    stats.access ++;
    vector<int> parsed_addr = parse_address(addr);
    int tagValue = parsed_addr[0];
    int indexValue = parsed_addr[1];
    int byteOffsetValue = parsed_addr[2];
    if(configDetails.blockSize - byteOffsetValue < size){
        load_error = -4;
        return "";
    }
    // checking if any valid block's tag is matching with our tag or not
    int blockNo;
    for(blockNo = 0; blockNo < configDetails.associativity; blockNo++){
        if(cache[indexValue][blockNo].isValid && cache[indexValue][blockNo].tag == tagValue){
            break;
        }
    }
    // cache hit with 'blockNo' index value of vector in that row has that addr
    if(blockNo != configDetails.associativity){
        isHit = true;
    }
    // cache miss
    // compute the 'blockNo' - index of the vector element in that row where our data will be added in the cache
    else{
        // checks if some invalid block is available
        int j;
        for(j= 0; j< configDetails.associativity; j++){
            if(!cache[indexValue][j].isValid)
                break;
        }
        // some invalid block is available
        if(j!=configDetails.associativity){
            // get data into that block from memory and blockNo would be that 'j' value
            // update valid bit; tag, data(updated later)
            cache[indexValue][j].isValid = true;
            
            blockNo = j;
        }
        // all blocks are full, now they should be replaced if sets are present using replacement policies
        else{
            // direct-mapped cache
            if(configDetails.associativity == 1){
                blockNo = 0;
            }
            // random replacement policy 
            else if(configDetails.replacementPolicy == "RANDOM"){
                blockNo = rand()%configDetails.associativity; //from 0 to associativity - 1 (we're following 0-indexing)
            }
            // FIFO replacement policy
            else if(configDetails.replacementPolicy == "FIFO"){
                // among all blocks whichever has least rpOrder that would be blockNo
                blockNo = find_min_rpOrder(indexValue);
            }
            // LRU replacement policy
            else if(configDetails.replacementPolicy == "LRU"){
                // among all blocks whichever has least rpOrder that would be blockNo
                blockNo = find_min_rpOrder(indexValue);
            }
            // if block is dirty
            if(cache[indexValue][blockNo].isDirty){
                cache[indexValue][blockNo].isDirty = false; // make it clean
                long int prev_addr = ((cache[indexValue][blockNo].tag)*(configDetails.rows) + indexValue) * configDetails.blockSize;
                updateMemory(prev_addr, configDetails.blockSize, cache[indexValue][blockNo].data);
            }
        }
        // update tag, data with new values
        cache[indexValue][blockNo].tag = tagValue;
        cache[indexValue][blockNo].data = getDataFromMemory(addr);
        // If follows FIFO, update only when there is a miss
        if(configDetails.replacementPolicy == "FIFO"){
            cache[indexValue][blockNo].rpOrder = stats.access;
        }
    }
    // anytime we access memory, update LRU counter varaible
    if(configDetails.replacementPolicy == "LRU"){
        cache[indexValue][blockNo].rpOrder = stats.access;
    }
    // get hex data from cache based on size specified
    string hex_data;
    int i = byteOffsetValue;
    while(size!=0){
        hex_data.insert(hex_data.begin(),cache[indexValue][blockNo].data[2*configDetails.blockSize -1 - 2*i]);
        hex_data.insert(hex_data.begin(),cache[indexValue][blockNo].data[2*configDetails.blockSize -1 - 2*i-1]);
        i++;
        size--;
    }
    // updating hit and miss variables in cacheStats
    if(isHit)   stats.hit++;
    else    stats.miss++;
    // opening file_name.output file to store cache simulation data
    ofstream out(cache_output_filename, ios::app);     // opens the file to write
    // if any error in opening this file
    if (!out) {
        cout << "Error opening file!" << endl;
    }
    out << "R: Address: 0x" << std::hex << addr << ", Set: 0x" << indexValue;
    if(isHit) out << ", Hit, ";
    else out << ", Miss, ";
    out << "Tag: 0x" << std::hex << tagValue;
    if(cache[indexValue][blockNo].isDirty)  out << ", Dirty";
    else out << ", Clean";
    out << endl;
    out.close();
    return hex_data;
}

/* modifies data in cache in the address specified. It also takes data and size of the data to be written as arguments.
- It parses the given address to tag, index and byte offset
- Finds if its a cache hit or miss:
    - if hit:: in WT: update both memory and cache, in WB : update only cache and make that block's dirty bit to 1.
    - if its a miss:: in WT: update only memory(Since it is without allocate), in WB : we find the block number of any invalid block or will replace any block based on replacement policy. While replacing, if the block is dirty, we will update its data in memory and make it clean. Now, at this location it first gets data from memory and then updates data in that block with the argument's data. So, the block turned dirty.
- Access, hit and miss variables are updated accordingly
- This access details are written to output file.
*/
void storeDataInMemory(long int addr, int size, string data){
    bool isHit = false;
    // since its accessing cache to check if that addr is present in cache or not
    stats.access ++;
    vector<int> parsed_addr = parse_address(addr);
    int tagValue = parsed_addr[0];
    int indexValue = parsed_addr[1];
    int byteOffsetValue = parsed_addr[2];
    if(configDetails.blockSize - byteOffsetValue < size){
        load_error = -4;
        return;
    }
    // checking if any valid block's tag is matching with our tag or not
    int blockNo;
    for(blockNo = 0; blockNo < configDetails.associativity; blockNo++){
        if(cache[indexValue][blockNo].isValid && cache[indexValue][blockNo].tag == tagValue){
            break;
        }
    }
    // cache hit with 'blockNo' index value of vector in that row has that addr
    if(blockNo != configDetails.associativity){
        isHit = true;
        if(configDetails.replacementPolicy == "LRU"){
            cache[indexValue][blockNo].rpOrder = stats.access;
        }
        // cache update in both policies
        updateCache(indexValue, blockNo, byteOffsetValue, size, data);
        
        // if policy is WT - update memory also
        if(configDetails.writeBackPolicy == "WT"){
            updateMemory(addr, size, data);
        }
        else if(configDetails.writeBackPolicy == "WB"){
            cache[indexValue][blockNo].isDirty = true;  // made that block dirty
        }
    }
    // cache miss
    // compute the 'blockNo' - index of the vector element in that row where our data will be added in the cache
    else{
        // write through in case of a miss: No allocate
        if(configDetails.writeBackPolicy == "WT"){
            updateMemory(addr, size, data);
        }
        // if write back : with allocate
        else if(configDetails.writeBackPolicy == "WB"){
            
            // checks if some invalid block is available
            for(blockNo= 0; blockNo< configDetails.associativity; blockNo++){
                if(!cache[indexValue][blockNo].isValid)
                    break;
            }
            // case-1: some invalid block is available
            if(blockNo!=configDetails.associativity){
                // get data into that block from memory and blockNo would be that 'j' value
                // update valid bit; tag, data(updated later)
                cache[indexValue][blockNo].isValid = true;
            }
            // case-2: some block has to be replaced
            else{
            // direct-mapped cache
            if(configDetails.associativity == 1){
                blockNo = 0;
            }
            // random replacement policy
            else if(configDetails.replacementPolicy == "RANDOM"){
                blockNo = rand()%configDetails.associativity; //from 0 to associativity - 1 (we're following 0-indexing)
            }
            // FIFO replacement policy
            else if(configDetails.replacementPolicy == "FIFO"){
                // among all blocks whichever has least rpOrder that would be blockNo
                blockNo = find_min_rpOrder(indexValue);
            }
            // LRU replacement policy
            else if(configDetails.replacementPolicy == "LRU"){
                // among all blocks whichever has least rpOrder that would be blockNo
                blockNo = find_min_rpOrder(indexValue);
            }
            // if block is dirty
            if(cache[indexValue][blockNo].isDirty){
                cache[indexValue][blockNo].isDirty = false; // make it clean
                long int prev_addr = ((cache[indexValue][blockNo].tag)*(configDetails.rows) + indexValue) * configDetails.blockSize;
                updateMemory(prev_addr, configDetails.blockSize, cache[indexValue][blockNo].data);
            }
        }
        if(configDetails.replacementPolicy == "FIFO" || configDetails.replacementPolicy == "LRU"){
            cache[indexValue][blockNo].rpOrder = stats.access;
        }
        cache[indexValue][blockNo].tag = tagValue;
        cache[indexValue][blockNo].data = getDataFromMemory(addr);
        cache[indexValue][blockNo].isDirty = true; // make it dirty
        updateCache(indexValue, blockNo, byteOffsetValue, size, data);
        }
    }

    // updating hit and miss variables in cacheStats
    if(isHit)   stats.hit++;
    else    stats.miss++;
    // opening file_name.output file to store cache simulation data
    ofstream out(cache_output_filename, ios::app);     // opens the file to write
    // if any error in opening this file
    if (!out) {
        cout << "Error opening file!" << endl;
    }
    out << "W: Address: 0x" << std::hex << addr << ", Set: 0x" << indexValue;
    if(isHit) out << ", Hit, ";
    else out << ", Miss, ";
    out << "Tag: 0x" << std::hex << tagValue;
    if(cache[indexValue][blockNo].isDirty)  out << ", Dirty" ;
    else out << ", Clean" ;
    out << endl;
    out.close();
}

