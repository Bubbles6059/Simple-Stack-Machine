#include "bof.h"
#include "instruction.h"
#include "machine_types.h"
#include "string.h"
#include <stdio.h>
#include <stdlib.h>

#define LO 0
#define HI 1

// memory size 2^16 bytes
#define MEMORY_SIZE_IN_BYTES (65536 - BYTES_PER_WORD)
#define MEMORY_SIZE_IN_WORDS (MEMORY_SIZE_IN_BYTES / BYTES_PER_WORD)

//Both used to check for the file name and the flag if you were to place it anywhere in the command line. 
bool pFlagChecker(int arraySize, char** stringArray);
char* findFileName(int arraySize, char** stringArray);

int main(int argc, char** argv) 
{
    bin_instr_t* instructArray = malloc(sizeof(MEMORY_SIZE_IN_WORDS));
    
    BOFFILE boffile = bof_read_open(findFileName(argc, argv));
    //BOFHeader bofHeader = bof_read_header(boffile);
    
    if(pFlagChecker(argc, argv)) 
    {
        
        for(int i = 0; i < MEMORY_SIZE_IN_WORDS; i++) 
        {
            instructArray[i] = instruction_read(boffile);
        }
        /*
        for(int i = 0; i < MEMORY_SIZE_IN_WORDS; i++) 
        {
            printf("%s\n", instruction_mnemonic(instructArray[i]));
        }
        */

        //bin_instr_t instruct = instruction_read(boffile);
        
    }
    return 0;
}

bool pFlagChecker(int arraySize, char** stringArray) 
{
    for(int i = 0; i < arraySize; i++) 
    {
        if(strcmp(stringArray[i], "-p") == 0)
            return true;
    }
    return false;
}

char* findFileName(int arraySize, char** stringArray) 
{
    for(int i = 0; i < arraySize; i++) 
    {
        if(strcmp(stringArray[i], "./vm") != 0 && strcmp(stringArray[i], "-p") != 0)
            return stringArray[i];
    }
    return "";
}