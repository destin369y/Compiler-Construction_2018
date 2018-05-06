/*  machine.c
 *  Source code for low level operations used in a simulator for
 *  the ILOC subset defined in
 *  "Engineering a Compiler" by Cooper and Torczon
 *  written by Todd Waterman
 *  11/30/00 */

#include <stdlib.h>
#include <stdio.h>
#include "machine.h"

int cycle_count;

/* Initialize arrays used in the machine during the simulation */
void initialize_machine(int reg_size, int mem_size)
{
    int i;
    
    if (reg_size == 0)
	NUM_REGISTERS = DEFAULT_NUM_REGISTERS;
    else 
	NUM_REGISTERS = reg_size;

    if (mem_size == 0)
	MEMORY_SIZE = DEFAULT_MEMORY_SIZE;
    else
	MEMORY_SIZE = mem_size;

    register_bank = malloc(NUM_REGISTERS*sizeof(int));

    for(i=0;i<NUM_REGISTERS;i++)
	register_bank[i] = 0;


    memory_bank = (char*)malloc(MEMORY_SIZE*sizeof(char));

    for(i=0;i<MEMORY_SIZE;i++)
	memory_bank[i] = 0;

}

/* These functions allow word (integer) access to memory, a word of memory
   is assumed to be 4 bytes */
int get_word(int location)
{
  int value;

  if ((location %4) != 0) {
    fprintf(stderr,"Simulator error: attempt to read an unaligned word.\n");
    exit(-1);
  }
  
  value = ((int)(unsigned char)get_memory(location) << 24 | 
		 (int)(unsigned char)get_memory(location+1) << 16 |
		 (int)(unsigned char)get_memory(location+2) << 8 | 
		 (int)(unsigned char)get_memory(location+3));
    return(value);
}

void set_word(int location, int value) {
  if ((location %4) != 0) {
    fprintf(stderr,"Simulator error: attempt to set an unaligned word.\n");
    exit(-1);
  }

  set_memory(location,(char)(value >> 24));
  set_memory(location+1,(char)((value << 8) >> 24));
  set_memory(location+2,(char)((value << 16) >> 24));
  set_memory(location+3,(char)((value << 24) >> 24));
}


/* The following functions manage access to the register and memory banks,
   this prevents the simulator from crashing if the input program uses an 
   invalid memory address or register name */

int get_register(int reg)
{
    if (reg >= 0 && reg < NUM_REGISTERS)
	return register_bank[reg];
    
    fprintf(stderr,"Simulator Error: Invalid register number r%d used in cycle %d.\n", 
	    reg,cycle_count);
    exit(1);
}
	
void set_register(int reg, int value)
{
    if (reg >= 0 && reg < NUM_REGISTERS)
	register_bank[reg] = value;
    else
    {
      fprintf(stderr,"Simulator Error: Invalid register number r%d used in cycle %d.\n", 
	      reg,cycle_count);
	exit(1);
    }
}


char get_memory(int location)
{
    if (location >= 0 && location < MEMORY_SIZE)
	return memory_bank[location];
    
    fprintf(stderr,"Simulator Error: Invalid memory address %d accessed in cycle %d.\n", 
	    location,cycle_count);
    exit(1);
}

void set_memory(int location,char value)
{
    if (location >= 0 && location < MEMORY_SIZE)
	memory_bank[location] = value;
    else
    {
      fprintf(stderr,"Simulator Error: Invalid memory address %d accessed in cycle %d.\n",
	      location,cycle_count);
      exit(1);
    }
}


