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

    // loading program
    if(pFlagChecker(argc, argv)) 
    {
        // *** add in \n after 59 char here ***

        printf("Address Instruction\n");

		// printing instructions
        for(int i = 0; i < bofHeader.text_length; i++) 
        {
            printf("%6d: %s\n", i, instruction_assembly_form(i, memory.instrs[i]));
        }
		/*
		int prev_word = -1;
		int bf_word = -1;
		int next_word = -1;
		*/
		int zeroCounter = 0;
		int i = 0;
		for (i = 0; i < bofHeader.data_length; i++) 
		{
			/*
			word_type bofWord = bof_read_word(boffile);
			if(bofWord == 0)
				zeroCounter++;
			if(zeroCounter < 2)
				printf("%8d: %d", bofHeader.data_start_address + i, bofWord);
			else
				break;
			*/
		}


		// printing data and words
        /*for (int i = 0; i < bofHeader.data_length; i++) 
        {	
			bf_word = bof_read_word(boffile);

			// check next word exists
			if ((i + 1) < bofHeader.data_length)
			{
				next_word = bof_read_word(boffile); 	
			}
			else
			{
				next_word = -1;
			}
			
			// print only current if on first word or last
			if (prev_word == -1 || next_word == -1)
			{
				printf("%8d: %d\t", bofHeader.data_start_address + i, bf_word);
			}
			
			// print current and next
			else if ((prev_word == 0) && (bf_word == 0) && (next_word == 0))
			{
				// 3 zeros in a row print ellipses
				i++;
				printf("...");
			}
			else
			{
				// otherwise print normal
            	printf("%8d: %d\t", bofHeader.data_start_address + i, bf_word);
				i++;
				printf("%8d: %d\t", bofHeader.data_start_address + i, bf_word);
			}
			
			prev_word = next_word;
        }*/

		// *** delete if not needed ***
        //bin_instr_t instruct = instruction_read(boffile);
    }

    // executing program and printing tracing output
    else
    {
        // *** add in \n after 59 char here ***

        for(int i = 0; i < bofHeader.text_length; i++) 
        {
			address_type PC = bofHeader.text_start_address;
			word_type HI = 1;
			word_type LO = 0;
			word_type* GPR = malloc(sizeof(word_type) * 8);
			//Initialize GPR
			for(int i = 0; i < 8; i++) {
				GPR[i] = 0;
			}
			GPR[0] = bofHeader.data_start_address; //$gp
			GPR[1] = bofHeader.stack_bottom_addr; //$sp
			GPR[2] = bofHeader.stack_bottom_addr; //$fp 
			
			// *** PC value not working ***
			PC+=i;
            printf("        PC: %d\n", PC);

            bin_instr_t instruct = memory.instrs[i];
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
						memory.words[GPR[t] + machine_types_formOffset(ot)] = memory.words[GPR[1]] + memory.words[GPR[s] + machine_types_formOffset(os)];
					break;

					case SUB_F:
						memory.words[GPR[t] + machine_types_formOffset(ot)] = memory.words[GPR[1]] - memory.words[GPR[s] + machine_types_formOffset(os)];
					break;

					case CPW_F:
						memory.words[GPR[t] + machine_types_formOffset(ot)] = memory.words[GPR[s] + machine_types_formOffset(os)];
					break;

					case AND_F:
						memory.uwords[GPR[t] + machine_types_formOffset(ot)] = memory.uwords[GPR[1]] & memory.uwords[GPR[s] + machine_types_formOffset(os)];
					break;

					case BOR_F:
						memory.uwords[GPR[t] + machine_types_formOffset(ot)] = memory.uwords[GPR[1]] | memory.uwords[GPR[s] + machine_types_formOffset(os)];
					break;

					case NOR_F:
						memory.uwords[GPR[t] + machine_types_formOffset(ot)] = ~(memory.uwords[GPR[1]] | memory.uwords[GPR[s] + machine_types_formOffset(os)]);
					break;

					case XOR_F:
						memory.uwords[GPR[t] + machine_types_formOffset(ot)] = memory.uwords[GPR[1]] ^ memory.uwords[GPR[s] + machine_types_formOffset(os)];
					break;

					case LWR_F:
						GPR[t] = memory.words[GPR[s] + machine_types_formOffset(os)];
					break;

					case SWR_F:
						memory.words[GPR[t] + machine_types_formOffset(ot)] = GPR[s];
					break;

					case SCA_F:
						memory.words[GPR[t] + machine_types_formOffset(ot)] = GPR[s] + machine_types_formOffset(os);
					break;

					case LWI_F:
						memory.words[GPR[t] + machine_types_formOffset(ot)] = memory.words[memory.words[GPR[s] + machine_types_formOffset(os)]];
					break;

					case NEG_F:
						memory.words[GPR[t] + machine_types_formOffset(ot)] = -memory.words[GPR[s] + machine_types_formOffset(os)];
					break;

					default:
						printf("Temporary Default");
					break;
					}
	            }
				break;
				case other_comp_instr_type:
				{
					other_comp_instr_t oco = instruct.othc;
					word_type op = instruct.othc.op;
					word_type reg = instruct.othc.reg;
					offset_type ot = instruct.othc.offset;
					arg_type arg = instruct.othc.arg;

					switch(oco.func) {
						case LIT_F:
							memory.words[GPR[reg] + ot] = machine_types_sgnExt(arg);
						break;
						
						case ARI_F:
							GPR[reg] = GPR[reg] + machine_types_sgnExt(arg);
						break;

						case SRI_F:
							GPR[reg] = GPR[reg] - machine_types_sgnExt(arg);
						break;

						case MUL_F:
							{
							//Seperate HI and LO
							int64_t result = (int64_t)((memory.words[GPR[reg] + machine_types_formOffset(ot)]) * (memory.words[GPR[1]]));
							HI = (result >> 32);
							LO = result;
							}
						break;

						case DIV_F:
							HI = memory.words[GPR[1]] % (memory.words[GPR[reg] + machine_types_formOffset(ot)]);
							LO = memory.words[GPR[1]] / (memory.words[GPR[reg] + machine_types_formOffset(ot)]);
						break;

						case CFHI_F:
							memory.words[GPR[reg] + machine_types_formOffset(ot)] = HI;
						break;

						case CFLO_F:
							memory.words[GPR[reg] + machine_types_formOffset(ot)] = LO;
						break;

						case SLL_F:
							memory.uwords[GPR[reg] + machine_types_formOffset(ot)] = memory.uwords[GPR[1]] << arg;
						break;

						case SRL_F:
							memory.uwords[GPR[reg] + machine_types_formOffset(ot)] = memory.uwords[GPR[1]] >> arg;
						break;

						case JMP_F:
							PC = memory.uwords[GPR[reg] + machine_types_formOffset(ot)];
						break;

						case CSI_F:
							GPR[7] = PC;
							PC = memory.words[GPR[reg] + machine_types_formOffset(ot)];
						break;

						case JREL_F:
							PC = ((PC - 1) + machine_types_formOffset(ot));
						break;
						//See table seven for last thing
						case SYS_F:
						{
							syscall_instr_t sys = instruct.syscall;
							offset_type ot = sys.offset;
							word_type reg = sys.reg;
							
							switch(sys.func) {
								case exit_sc:
									exit(machine_types_sgnExt(ot));
								break;

								case print_str_sc:
									memory.words[GPR[1]] = printf("%s", &memory.words[GPR[reg] + machine_types_formOffset(ot)]);
								break;

								case print_int_sc:
									memory.words[GPR[1]] = printf("%d", memory.words[GPR[reg] + machine_types_formOffset(ot)]);
								break;

								case print_char_sc:
									memory.words[GPR[1]] = fputc(memory.words[GPR[reg] + machine_types_formOffset(ot)], stdout);
								break;

								case read_char_sc:
									memory.words[GPR[reg] + machine_types_formOffset(ot)] = getc(stdin);
								break; 

								case start_tracing_sc:
								break;

								case stop_tracing_sc:
								break;								
							}
						}
						break;
					}
				}
				
			case immed_instr_type:
			{
				immed_instr_t imm = instruct.immed;
				word_type reg = imm.reg;
				word_type ot = imm.offset;
				immediate_type immed = imm.immed;
				switch(imm.op)
				{
					case ADDI_O:
						memory.words[GPR[reg] + machine_types_formOffset(ot)] = memory.words[GPR[reg] + machine_types_formOffset(ot) + machine_types_sgnExt(immed)];
				
					break;

					case ANDI_O:
						memory.uwords[GPR[reg] + machine_types_formOffset(ot)] = memory.uwords[GPR[reg] + machine_types_formOffset(ot) & machine_types_zeroExt(immed)];
					break;

					case BORI_O:
						memory.uwords[GPR[reg] + machine_types_formOffset(ot)] = memory.uwords[GPR[reg] + machine_types_formOffset(ot) | machine_types_zeroExt(immed)];
					break;

					case NORI_O:
						memory.uwords[GPR[reg] + machine_types_formOffset(ot)] = !memory.uwords[GPR[reg] + machine_types_formOffset(ot) | machine_types_zeroExt(immed)];
					break;

					case XORI_O:
						memory.uwords[GPR[reg] + machine_types_formOffset(ot)] = memory.uwords[GPR[reg] + machine_types_formOffset(ot) ^ machine_types_zeroExt(immed)];
					break;

					case BEQ_O:
						if (memory.words[GPR[1] = memory.words[GPR[reg] + machine_types_formOffset(ot)]]){
							PC = ((PC - 1) + machine_types_formOffset(immed));
						}
					break;

					case BGEZ_O:
						if (memory.words[GPR[reg] + machine_types_formOffset(ot)] >= 0){
							PC = ((PC - 1) + machine_types_formOffset(immed));
						}
					break;

					case BGTZ_O:
					if (memory.words[GPR[reg] + machine_types_formOffset(ot)] > 0 ){
							PC = ((PC - 1) + machine_types_formOffset(immed));
						}
					break;

					case BLEZ_O:
					if (memory.words[GPR[reg] + machine_types_formOffset(ot)] <= 0 ){
							PC = ((PC - 1) + machine_types_formOffset(immed));
						}
					break;

					case BLTZ_O:
					if (memory.words[GPR[reg] + machine_types_formOffset(ot)] < 0 ){
							PC = ((PC - 1) + machine_types_formOffset(immed));
						}
				
					break;

					case BNE_O:
					if (memory.words[GPR[1] != memory.words[GPR[reg] + machine_types_formOffset(ot)]]){
						PC = ((PC - 1) + machine_types_formOffset(immed));
					}

					break;
				}
			}

		}	

			// *** change to 59 char \n ***
			for (int i = 0; i < 8; i++) 
			{
        		printf("GPR[%s]: %-8d", regname_get(i), GPR[i]);
        		if ((i + 1) % 5 == 0) 
					printf("\n");
    		}

			printf("\n\n");
			// moved above printf("==>") as opposed to below
			bool printing_zeros = false;
			for(int i = GPR[0]; i <= GPR[1]; i++) 
			{
				if (memory.words[i] == 0) 
				{
            		if (!printing_zeros) 
					{
                		printf("%d:  %-8d\t...\n", i, memory.words[i]);
						printf("%d:  %-8d\t", bofHeader.stack_bottom_addr, memory.words[i]);
                		printing_zeros = true;
            		}
				}
				else
					printf("%8d: %-8d\n", i, memory.words[i]);
					//printing_zeros
			}

            printf("==> %6d: %s\n", i, instruction_assembly_form(i, memory.instrs[i]));

			// *** prints dots incorrectly ***
        }
    }

    return 0;
}