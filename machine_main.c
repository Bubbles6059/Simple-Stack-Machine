#include "bof.h"
#include "instruction.h"
#include "machine_types.h"
#include "string.h"
#include <stdio.h>
#include <stdlib.h>

// memory size 2^16 words
#define MEMORY_SIZE_IN_WORDS 32768
#define LO 0
#define HI 1

// *** add comment ***
static union mem_u {
word_type words[MEMORY_SIZE_IN_WORDS];
uword_type uwords[MEMORY_SIZE_IN_WORDS];
bin_instr_t instrs[MEMORY_SIZE_IN_WORDS];
} memory;

// processing arguments
char* findFileName(int arraySize, char** stringArray);
bool pFlagChecker(int arraySize, char** stringArray);

char* findFileName(int arraySize, char** stringArray) 
{
    for(int i = 0; i < arraySize; i++) 
    {
        if(strcmp(stringArray[i], "./vm") != 0 && strcmp(stringArray[i], "-p") != 0)
            return stringArray[i];
    }
    return "";
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

int main(int argc, char** argv) 
{
    //bin_instr_t* instructArray = malloc(sizeof(MEMORY_SIZE_IN_WORDS));
    
    BOFFILE boffile = bof_read_open(findFileName(argc, argv));
    BOFHeader bofHeader = bof_read_header(boffile);
    
    // loading program and printing
    if(pFlagChecker(argc, argv)) 
    {
        for(int i = 0; i < bofHeader.text_length; i++) 
        {
            memory.instrs[i] = instruction_read(boffile);
        }

        // *** add in \n after 57 char here ***

        printf("Address Instruction\n");

        for(int i = 0; i < bofHeader.text_length; i++) 
        {
            printf("%6d: %s\n", i, instruction_assembly_form(i, memory.instrs[i]));
        }
        for(int i = 0; i < bofHeader.data_length; i++) 
        {
            // *** cut off after 2 consecutive zeros here ***
            printf("%8d: %d\t", bofHeader.data_start_address + i, bof_read_word(boffile));
        }

        //bin_instr_t instruct = instruction_read(boffile);

        printf("        ...");
    }

    // executing program and printing tracing output
    else
    {
        // *** execute program here ***
        /*
            references
            machine_execute_instr(bin_instr_t bi) in machine.c
        */

        // *** add in \n after 57 char here ***

        for(int i = 0; i < bofHeader.text_length; i++) 
        {
            printf("        PC: %d\n", i);
            // *** print GPR[$gp, $sp, $fp, $r3, $r4, $r5, $r6, $ra] here ***
            /*  
                references 
                unparseReg(reg_num_type n)  asm_unparser.c
                regname_get                 regname.c 

            */

            printf("\n");
            printf("==> %6d: %s\n", i, instruction_assembly_form(i, memory.instrs[i]));
        }
    }

    return 0;
}