/*  machine.h
 *  Header file for low level operations used in a simulator for
 *  the ILOC subset defined in
 *  "Engineering a Compiler" by Cooper and Torczon
 *  written by Todd Waterman
 *  11/30/00 */

#ifndef _MACHINE_H_
#define _MACHINE_H_

/* The default number of bytes of addressable memory starting from 0 */
#define DEFAULT_MEMORY_SIZE 4000000
int MEMORY_SIZE;

/* The default number of registers */
#define DEFAULT_NUM_REGISTERS 1000
int NUM_REGISTERS;

/* Array of registers */
int* register_bank;

/* A character array that represents main memory */
char* memory_bank;

/* Initialize arrays used in the machine representation during the 
   simulation */
void initialize_machine(int reg_size, int mem_size);

/* These functions allow word (integer) access to memory, a word of memory
   is assumed to be 4 bytes */
int get_word(int);
void set_word(int,int);

/* The following functions manage access to the register and memory banks,
   this prevents the simulator from crashing if the input program uses an 
   invalid memory address or register name */
int get_register(int);
void set_register(int,int);
char get_memory(int);
void set_memory(int,char);

#endif /* _MACHINE_H_ */   
