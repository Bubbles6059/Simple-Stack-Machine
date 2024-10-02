// COP 3402 Group 16
// Alyssa Buckingham
// 
//
//
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
    BOFFILE boffile = bof_read_open(findFileName(argc, argv));
    BOFHeader bofHeader = bof_read_header(boffile);
    for(int i = 0; i < bofHeader.text_length; i++) 
    {            
		memory.instrs[i] = instruction_read(boffile);
    }

    // Do the arguments match the flag checker function?
    if(pFlagChecker(argc, argv)) 
    {
		// Print the header.
        printf("Address Instruction\n");

		// Print the assembly form of instructions.
        for(int i = 0; i < bofHeader.text_length; i++) 
        {
            printf("%6d: %s\n", i, instruction_assembly_form(i, memory.instrs[i]));
        }

		// Print the data
		int zeroCounter = 0;
		int itemCounter = 0;

		word_type lastWord = 0;
		int i = 0;

		// Case for no words in the data.
		if (bofHeader.data_length == 0)
		{
			printf("%8d: %d\t", i + bofHeader.data_start_address, 0);
		}

		// Looping through the data and print each word.
        for (i = 0; i < bofHeader.data_length; i++) 
        {	
			// Reading a word from the BOF file.
			word_type bofWord = bof_read_word(boffile);

			// If the word is 0, increment zero counter.
			if (bofWord == 0) {
				zeroCounter++;
			}

			// If the word is not 0, reset the counter.
			else 
			{
				zeroCounter = 0;
			}

	
			if (zeroCounter == 2)
			{
				break;
			}
			else
			{
				if (itemCounter == 5)
				{
					printf("\n");
					itemCounter = 0;
				}
				printf("%8d: %d\t", bofHeader.data_start_address + i, bofWord);
				itemCounter++;
			}
			lastWord = bofWord;
        }
		if(lastWord != 0)
		{
			if (itemCounter == 5)
			{
				printf("\n");
				itemCounter = 0;
			}
			printf("%8d: %d\t", i + bofHeader.data_start_address, 0);
			itemCounter++;
		}
		

		if (itemCounter == 5)
		{
			printf("\n");
			itemCounter = 0;
		}
		printf("        ...");
		itemCounter++;

		for (int i = bofHeader.data_start_address; i < bofHeader.stack_bottom_addr; i++) {
			if (memory.words[i] != 0) {
				printf("%d: %-8d\t", i, memory.words[i]);
			}
		}

		

		printf("\n");
    }

    // executing program and printing tracing output
    else
    {
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
            printf("      PC: %d\n", PC);

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
					
					// Add and store register values.
	                case ADD_F:
						memory.words[GPR[t] + machine_types_formOffset(ot)] = memory.words[GPR[1]] + memory.words[GPR[s] + machine_types_formOffset(os)];
					break;

					// Subtract and store register values.
					case SUB_F:
						memory.words[GPR[t] + machine_types_formOffset(ot)] = memory.words[GPR[1]] - memory.words[GPR[s] + machine_types_formOffset(os)];
					break;

					// Copies value from memeory to another
					case CPW_F:
						memory.words[GPR[t] + machine_types_formOffset(ot)] = memory.words[GPR[s] + machine_types_formOffset(os)];
					break;

					// Bitwise AND between two registers and store the result.
					case AND_F:
						memory.uwords[GPR[t] + machine_types_formOffset(ot)] = memory.uwords[GPR[1]] & memory.uwords[GPR[s] + machine_types_formOffset(os)];
					break;

					// Bitwise OR between two registers and store the result.
					case BOR_F:
						memory.uwords[GPR[t] + machine_types_formOffset(ot)] = memory.uwords[GPR[1]] | memory.uwords[GPR[s] + machine_types_formOffset(os)];
					break;

					// Bitwise NOR between two registers and store the result.
					case NOR_F:
						memory.uwords[GPR[t] + machine_types_formOffset(ot)] = ~(memory.uwords[GPR[1]] | memory.uwords[GPR[s] + machine_types_formOffset(os)]);
					break;

					// Bitwise XOR between two registers and store the result.
					case XOR_F:
						memory.uwords[GPR[t] + machine_types_formOffset(ot)] = memory.uwords[GPR[1]] ^ memory.uwords[GPR[s] + machine_types_formOffset(os)];
					break;

					// Load into a register.
					case LWR_F:
						GPR[t] = memory.words[GPR[s] + machine_types_formOffset(os)];
					break;

					// Store a register value.
					case SWR_F:
						memory.words[GPR[t] + machine_types_formOffset(ot)] = GPR[s];
					break;

					// Stire the sum of a register and an offset.
					case SCA_F:
						memory.words[GPR[t] + machine_types_formOffset(ot)] = GPR[s] + machine_types_formOffset(os);
					break;

				
					case LWI_F:
						memory.words[GPR[t] + machine_types_formOffset(ot)] = memory.words[memory.words[GPR[s] + machine_types_formOffset(os)]];
					break;

					// Negate a value from a register and store the result.
					case NEG_F:
						memory.words[GPR[t] + machine_types_formOffset(ot)] = -memory.words[GPR[s] + machine_types_formOffset(os)];
					break;

					default:
            		// Handle unknown function code
            		bail_with_error("Unknown function code in computational instruction");
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

						// Subtract immediate value from general-purpose register.
						case SRI_F:
							GPR[reg] = GPR[reg] - machine_types_sgnExt(arg);
						break;

						// Multiply two values, and store into HI and LO.
						case MUL_F:
							{
							// Seperate HI and LO.
							int64_t result = (int64_t)((memory.words[GPR[reg] + machine_types_formOffset(ot)]) * (memory.words[GPR[1]]));
							HI = (result >> 32);
							LO = result;
							}
						break;

						// Divide two values and store the remainder and quotient.
						case DIV_F:
							HI = memory.words[GPR[1]] % (memory.words[GPR[reg] + machine_types_formOffset(ot)]);
							LO = memory.words[GPR[1]] / (memory.words[GPR[reg] + machine_types_formOffset(ot)]);
						break;

						// Copies the value in HI register.
						case CFHI_F:
							memory.words[GPR[reg] + machine_types_formOffset(ot)] = HI;
						break;

						// Copies the value in LO register.
						case CFLO_F:
							memory.words[GPR[reg] + machine_types_formOffset(ot)] = LO;
						break;

						// Preform a logical left shift.
						case SLL_F:
							memory.uwords[GPR[reg] + machine_types_formOffset(ot)] = memory.uwords[GPR[1]] << arg;
						break;

						// Preforms a logical right shift.
						case SRL_F:
							memory.uwords[GPR[reg] + machine_types_formOffset(ot)] = memory.uwords[GPR[1]] >> arg;
						break;

						// Jump to the address contained in memory.
						case JMP_F:
							PC = memory.uwords[GPR[reg] + machine_types_formOffset(ot)];
						break;

						// Save PC and jump to new address.
						case CSI_F:
							GPR[7] = PC;
							PC = memory.words[GPR[reg] + machine_types_formOffset(ot)];
						break;

						// Jump to a relative address based on PC and offset.
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

								// Exits.
								case exit_sc:
									exit(machine_types_sgnExt(ot));
								break;

								// Print a string from memory.
								case print_str_sc:
									memory.words[GPR[1]] = printf("%s", &memory.words[GPR[reg] + machine_types_formOffset(ot)]);
								break;

								// Print an integer from memory.
								case print_int_sc:
									memory.words[GPR[1]] = printf("%d", memory.words[GPR[reg] + machine_types_formOffset(ot)]);
								break;

								// Print a character from memory.
								case print_char_sc:
									memory.words[GPR[1]] = fputc(memory.words[GPR[reg] + machine_types_formOffset(ot)], stdout);
								break;

								// Read and sto character from standard input
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
					// Adding an immediate value (sign-extended) to a memory location.
					case ADDI_O:
						memory.words[GPR[reg] + machine_types_formOffset(ot)] = memory.words[GPR[reg] + machine_types_formOffset(ot) + machine_types_sgnExt(immed)];
				
					break;

					// Bitwise AND immediate value with a register.
					case ANDI_O:
						memory.uwords[GPR[reg] + machine_types_formOffset(ot)] = memory.uwords[GPR[reg] + machine_types_formOffset(ot) & machine_types_zeroExt(immed)];
					break;

					// Biwise OR immediate value with a register.
					case BORI_O:
						memory.uwords[GPR[reg] + machine_types_formOffset(ot)] = memory.uwords[GPR[reg] + machine_types_formOffset(ot) | machine_types_zeroExt(immed)];
					break;

					// Bitwise NOR immediate value with a register.
					case NORI_O:
						memory.uwords[GPR[reg] + machine_types_formOffset(ot)] = ~(memory.uwords[GPR[reg] + machine_types_formOffset(ot)]) | machine_types_zeroExt(immed);
					break;

					// Bitwise XOR immediate value with a register.
					case XORI_O:
						memory.uwords[GPR[reg] + machine_types_formOffset(ot)] = memory.uwords[GPR[reg] + machine_types_formOffset(ot) ^ machine_types_zeroExt(immed)];
					break;

					// Branch if equal, if the register values are equal, PC is updated.
					case BEQ_O:
						if (memory.words[GPR[1] = memory.words[GPR[reg] + machine_types_formOffset(ot)]]){
							PC = ((PC - 1) + machine_types_formOffset(immed));
						}

					break;

					// Branch if greater than or equal to zero, if register value is greater than or equal to zero, branch.
					case BGEZ_O:
						if (memory.words[GPR[reg] + machine_types_formOffset(ot)] >= 0){
							PC = ((PC - 1) + machine_types_formOffset(immed));
						}

					break;

					// Branch if greater than zero, if register value is greater than zero, branch.
					case BGTZ_O:
					if (memory.words[GPR[reg] + machine_types_formOffset(ot)] > 0 ){
							PC = ((PC - 1) + machine_types_formOffset(immed));
						}

					break;

					// Branch if less than or equal to zero, if register value is less than or equal to zero, branch.
					case BLEZ_O:
					if (memory.words[GPR[reg] + machine_types_formOffset(ot)] <= 0 ){
							PC = ((PC - 1) + machine_types_formOffset(immed));
						}

					break;

					// Branch if less than zero, if the register value is less than zero, branch.
					case BLTZ_O:
					if (memory.words[GPR[reg] + machine_types_formOffset(ot)] < 0 ){
							PC = ((PC - 1) + machine_types_formOffset(immed));
						}
				
					break;

					// Branch if not equal.
					case BNE_O:
					if (memory.words[GPR[1] != memory.words[GPR[reg] + machine_types_formOffset(ot)]]){
						PC = ((PC - 1) + machine_types_formOffset(immed));
					}
					
					break;
				}
			}
			break;

			case jump_instr_type:
			{
				jump_instr_t jmp = instruct.jump;
				word_type reg = jmp.addr;
				word_type op = jmp.op;

				switch(op) 
				{
					case JMPA_O:
						PC = machine_types_formAddress(PC-1, reg);
					break;

					case CALL_O:
						GPR[7] = PC;
						PC = machine_types_formAddress(PC-1, reg);
					break;

					case RTN_O:
						PC = GPR[7];
					break;
				}
				
			}

		}	

			for (int i = 0; i < 8; i++) 
			{
        		printf("GPR[%s]: %-6d", regname_get(i), GPR[i]); // prints the name of the
        		if ((i + 1) % 5 == 0) 
					printf("\n");
    		}
			
			printf("\n");
			
			bool printing_zeros = false;
			for(int i = GPR[0]; i <= GPR[1]; i++) 
			{
				if (memory.words[i] == 0) 
				{
            		if (!printing_zeros) 
					{
                		printf("%8d: %d\t...\n", i, memory.words[i]);
						printf("%8d: %d\t", bofHeader.stack_bottom_addr, memory.words[i]);
                		printing_zeros = true;
            		}
				}
				else
					printf("%8d: %-8d\n", i, memory.words[i]);
					//printing_zeros
				
				//for (int i = bofHeader.data_start_address; i <= bofHeader.stack_bottom_addr; i++) { // loops through the entire stack
				//	if (memory.words[i] != 0) // checks if memory.words[i] is zero
				//		printf("%d: %-8d\t", i, memory.words[i]);
				//}
			}
			
            printf("\n\n==> %6d: %s\n", i, instruction_assembly_form(i, memory.instrs[i]));

			// *** prints dots incorrectly ***
        }
    }

    return 0;
}
