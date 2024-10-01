#include "bof.h"
#include "instruction.h"
#include "machine_types.h"
#include "string.h"
#include "regname.h"
#include <math.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "utilities.h"
#include <stdbool.h>


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
    for(int i = 0; i < bofHeader.text_length; i++) 
        {
            memory.instrs[i] = instruction_read(boffile);
        }

    // loading program and printing
    if(pFlagChecker(argc, argv)) 
    {
        // *** add in \n after 59 char here ***

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

        // *** add in \n after 59 char here ***

        for(int i = 0; i < bofHeader.text_length; i++) 
        {
			address_type PC = bofHeader.text_start_address;
			word_type* GPR = malloc(sizeof(word_type) * 8);
			//Initialize GPR
			for(int i = 0; i < 8; i++) {
				GPR[i] = 0;
			}
			GPR[0] = bofHeader.data_start_address; //$gp
			GPR[1] = bofHeader.stack_bottom_addr; //$sp
			GPR[2] = bofHeader.stack_bottom_addr; //$fp 
            printf("        PC: %d\n", PC);

            bin_instr_t instruct = memory.instrs[i];
            /*
            instruct.comp;
            instruct.immed;
            instruct.jump;
            instruct.othc;
            instruct.syscall;
            instruct.uimmed;
            */
            instr_type it = instruction_type(instruct);

            switch (it) 
            {
                case comp_instr_type:
	            {
	                comp_instr_t co = instruct.comp;
					word_type t = instruct.comp.rt;
					word_type s = instruct.comp.rs;
					offset_type ot = instruct.comp.ot;
					offset_type os = instruct.comp.os;

	                switch (co.func) {
	                case ADD_F:
						memory.words[GPR[t] + ot] = memory.words[GPR[1]] + memory.words[GPR[s] + os];
					break;

					case SUB_F:
						memory.words[GPR[t] + ot] = memory.words[GPR[1]] - memory.words[GPR[s] + os];
					break;

					case CPW_F:
						memory.words[GPR[t] + ot] = memory.words[GPR[s] + os];
					break;

					case AND_F:
						memory.uwords[GPR[t] + ot] = memory.uwords[GPR[1]] & memory.uwords[GPR[s] + os];
					break;

					case BOR_F:
						memory.uwords[GPR[t] + ot] = memory.uwords[GPR[1]] | memory.uwords[GPR[s] + os];
					break;

					case NOR_F:
						memory.uwords[GPR[t] + ot] = ~(memory.uwords[GPR[1]] | memory.uwords[GPR[s] + os]);
					break;

					case XOR_F:
						memory.uwords[GPR[t] + ot] = memory.uwords[GPR[1]] ^ memory.uwords[GPR[s] + os];
					break;

					case LWR_F:
						GPR[t] = memory.words[GPR[s] + os];
					break;

					case SWR_F:
						memory.words[GPR[t] + ot] = GPR[s];
					break;

					case SCA_F:
						memory.words[GPR[t] + ot] = GPR[s] + os;
					break;

					case LWI_F:
						memory.words[GPR[t] + ot] = memory.words[memory.words[GPR[s] + os]];
					break;

					case NEG_F:
						memory.words[GPR[t] + ot] = -memory.words[GPR[s] + os];
					break;		 	
	                }

	            }
			break;
    }
            printf("\n");
            printf("==> %6d: %s\n", i, instruction_assembly_form(i, memory.instrs[i]));
			printf("%d", memory.words[i]);
        }
    }

    return 0;
}