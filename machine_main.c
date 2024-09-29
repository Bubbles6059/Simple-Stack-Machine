#include "bof.h"
#include "instruction.h"
#include "machine_types.h"
#include "string.h"
#include <stdio.h>
#include <stdlib.h>

#define LO 0
#define HI 1

// memory size 2^16 words
#define MEMORY_SIZE_IN_WORDS 32768

static union mem_u {
word_type words[MEMORY_SIZE_IN_WORDS];
uword_type uwords[MEMORY_SIZE_IN_WORDS];
bin_instr_t instrs[MEMORY_SIZE_IN_WORDS];
} memory;

//Both used to check for the file name and the flag if you were to place it anywhere in the command line. 
bool pFlagChecker(int arraySize, char** stringArray);
char* findFileName(int arraySize, char** stringArray);

int main(int argc, char** argv) 
{
    //bin_instr_t* instructArray = malloc(sizeof(MEMORY_SIZE_IN_WORDS));
    
    BOFFILE boffile = bof_read_open(findFileName(argc, argv));
    BOFHeader bofHeader = bof_read_header(boffile);
    
    if(pFlagChecker(argc, argv)) 
    {
        
        for(int i = 0; i < bofHeader.text_length; i++) 
        {
            memory.instrs[i] = instruction_read(boffile);
        }

        printf("Address Instruction\n");

        for(int i = 0; i < bofHeader.text_length; i++) 
        {
            printf("%d: %s\n", i, instruction_assembly_form(i, memory.instrs[i]));
        }
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