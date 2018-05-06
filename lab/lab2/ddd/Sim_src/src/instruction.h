/*  instruction.h
 *  written by Todd Waterman
 *  11/30/00 */

/*
 *  Added a HALT operation, April 2017, KDC
 *
 */

#ifndef _INSTRUCTION_H_
#define _INSTRUCTION_H_

#define LABELS_PER_ARRAY 20

/* This is the list of valid opcode names.
 * It should be kept synchronized with the opcode_specs table. */
typedef enum opcode_name {NOP=0, ADD, SUB, MULT, DIV, ADDI, SUBI, MULTI,
			  DIVI, LSHIFT, LSHIFTI, RSHIFT, RSHIFTI,
			  AND, ANDI, OR, ORI, NOT,
			  LOADI, LOAD, LOADAI, LOADAO, CLOAD, CLOADAI,
			  CLOADAO, STORE, STOREAI, STOREAO, CSTORE, 
			  CSTOREAI, CSTOREAO, BR, CBR, CMPLT, CMPLE,
			  CMPEQ, CMPNE, CMPGE, CMPGT, I2I, C2C, I2C, C2I,
			  OUTPUT, COUTPUT, READ, CREAD, WRITE, CWRITE, HALT } 
                          Opcode_Name;

/* The Opcode structure stores information about each argument */
typedef struct opcode {
    Opcode_Name name;
    char* string;
    int srcs, consts, labels, defs;
    int latency;
    int target_is_source;
} Opcode;

extern Opcode opcode_specs[];
extern int permitted_opcode[];

/* The Operand structure represents a single argument */
typedef struct operand {
    int value;
    struct operand* next;
} Operand;

/* The Operation structure represents a single opcode and its operands */
typedef struct operation {
    Opcode_Name opcode;
    Operand* srcs;
    Operand* consts;
    Operand* labels;
    Operand* defs;
    struct operation* next;
} Operation;

/* The Instruction structure represents a single instruction which can
   consist of several operations. */
typedef struct instruction {
    Operation* operations;
    struct instruction* next;
} Instruction;

/* The Label structure represents a label */
typedef struct label {
    int identifier;
    char* string;
    Instruction* target;
    struct label* next;
} Label;

/* The Label_Array structure keeps groups of pointers to various labels */
typedef struct label_array {
    Label* array[LABELS_PER_ARRAY];
    int first;
    struct label_array* next;
} Label_Array;

Label** hash_labels;
int number_of_labels;
Label_Array* label_list;


/* The Hashnode structure is used to create hash tables */
typedef struct hashnode {
    Opcode* value;
    struct hashnode* next;
} Hashnode;

Hashnode** hash_opcodes;

/* Run yyparse and return a pointer to the first instruction if no
   errors occur, otherwise return NULL */
Instruction* parse();


/* Initialize the tables needed for get_opcode */
void opcode_init(void);

/* Determine the opcode given the string */
Opcode* get_opcode(char*);

/* Insert label into label table and return index */
int insert_label(char*);

/* Return label pointer associated with the index */
Label* get_label(int);

/* Deallocate memory for a list of instructions */
void free_instructions(Instruction*);

/* Deallocate memory for a list of operations */
void free_operations(Operation*);

/* Deallocate memory for a list of operands */
void free_operands(Operand*);

/* The Operands structure is a temporary structure that is only used inside of 
   the parser to keep track of separate types of operands.  It is defined here 
   instead of within the yacc file because when Yacc creates the y.tab.h file that
   is used by lex it includes information about this structure even thoug the scanner
   does not need the information. */
typedef struct operands {
    int num_regs;
    Operand* regs;
    int num_consts;
    Operand* consts;
    int num_labels;
    Operand* labels;
} Operands;

#endif /* _INSTRUCTION_H_ */

