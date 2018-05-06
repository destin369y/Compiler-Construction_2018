/*  instruction.c
 *  written by Todd Waterman
 *  11/30/00 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "hash.h"
#include "instruction.h"

Opcode opcode_specs[] =
/*  name      string      srcs   consts labels defs   latency target_is_source */
/*  ------    ------      ------ ------ ------ ------ ------- ---------------- */

{
  { NOP,      "nop",      0,     0,     0,     0,     1,       0 },
  { ADD,      "add",      2,     0,     0,     1,     1,       0 },
  { SUB,      "sub",      2,     0,     0,     1,     1,       0 },
  { MULT,     "mult",     2,     0,     0,     1,     3,       0 },
  { DIV,      "div",      2,     0,     0,     1,     6,       0 },
  { ADDI,     "addI",     1,     1,     0,     1,     1,       0 },
  { SUBI,     "subI",     1,     1,     0,     1,     1,       0 },
  { MULTI,    "multI",    1,     1,     0,     1,     3,       0 },
  { DIVI,     "divI",     1,     1,     0,     1,     6,       0 },
  { LSHIFT,   "lshift",   2,     0,     0,     1,     1,       0 },
  { LSHIFTI,  "lshiftI",  1,     1,     0,     1,     1,       0 },
  { RSHIFT,   "rshift",   2,     0,     0,     1,     1,       0 },
  { RSHIFTI,  "rshiftI",  1,     1,     0,     1,     1,       0 },
  { AND,      "and",      2,     0,     0,     1,     1,       0 },
  { ANDI,     "andI",     1,     1,     0,     1,     1,       0 },
  { OR,       "or",       2,     0,     0,     1,     1,       0 },
  { ORI,      "orI",      1,     1,     0,     1,     1,       0 },
  { NOT,      "not",      1,     0,     0,     1,     1,       0 },
  { LOADI,    "loadI",    0,     1,     0,     1,     1,       0 },
  { LOAD,     "load",     1,     0,     0,     1,     5,       0 },
  { LOADAI,   "loadAI",   1,     1,     0,     1,     5,       0 },
  { LOADAO,   "loadAO",   2,     0,     0,     1,     5,       0 },
  { CLOAD,    "cload",    1,     0,     0,     1,     5,       0 },
  { CLOADAI,  "cloadAI",  1,     1,     0,     1,     5,       0 },
  { CLOADAO,  "cloadAO",  2,     0,     0,     1,     5,       0 },
  { STORE,    "store",    1,     0,     0,     1,     5,       1 },
  { STOREAI,  "storeAI",  1,     1,     0,     1,     5,       1 },
  { STOREAO,  "storeAO",  1,     0,     0,     2,     5,       1 },
  { CSTORE,   "cstore",   1,     0,     0,     1,     5,       1 },
  { CSTOREAI, "cstoreAI", 1,     1,     0,     1,     5,       1 },
  { CSTOREAO, "cstoreAO", 1,     0,     0,     2,     5,       1 },
  { BR,       "br",       0,     0,     1,     0,     1,       0 },
  { CBR,      "cbr",      1,     0,     2,     0,     1,       0 },
  { CMPLT,    "cmp_LT",   2,     0,     0,     1,     1,       0 },
  { CMPLE,    "cmp_LE",   2,     0,     0,     1,     1,       0 },
  { CMPEQ,    "cmp_EQ",   2,     0,     0,     1,     1,       0 },
  { CMPNE,    "cmp_NE",   2,     0,     0,     1,     1,       0 },
  { CMPGE,    "cmp_GE",   2,     0,     0,     1,     1,       0 },
  { CMPGT,    "cmp_GT",   2,     0,     0,     1,     1,       0 },
  { I2I,      "i2i",      1,     0,     0,     1,     1,       0 },
  { C2C,      "c2c",      1,     0,     0,     1,     1,       0 },
  { I2C,      "i2c",      1,     0,     0,     1,     1,       0 },
  { C2I,      "c2i",      1,     0,     0,     1,     1,       0 },
  { OUTPUT,   "output",   0,     1,     0,     0,     1,       0 },
  { COUTPUT,  "coutput",  0,     1,     0,     0,     1,       0 },
  { READ,     "read",     0,     0,     0,     1,     1,       0 },
  { CREAD,    "cread",    0,     0,     0,     1,     1,       0 },
  { WRITE,    "write",    1,     0,     0,     0,     1,       0 },
  { CWRITE,   "cwrite",   1,     0,     0,     0,     1,       0 },
  { HALT,     "halt",     0,     0,     0,     0,     1,       0 },
  { 0 } /* termination flag */
}; 

#define YES 1
#define NO  0

/* Array used to detect disallowed opcodes -- version specific tailoring */
int permitted_opcode[] = {
  /* nop */      YES,
  /* add */      YES,
  /* sub */      YES,
  /* mult */     YES,
  /* div */      YES,
  /* addI */     YES,
  /* subI */     YES,
  /* multI */    YES,
  /* divI */     YES,
  /* lshift */   YES,
  /* lshiftI */  YES,
  /* rshiftI */  YES,
  /* rshift */   YES,
  /* and */      YES,
  /* andI */     YES,
  /* or */       YES,
  /* orI */      YES,
  /* not */      YES,
  /* loadI */    YES,
  /* load */     YES,
  /* loadAI */   YES,
  /* loadAO */   YES,
  /* cload */    YES,
  /* cloadAI */  YES,
  /* cloadAO */  YES,
  /* store */    YES,
  /* storeAI */  YES,
  /* storeAO */  YES,
  /* cstore */   YES,
  /* cstoreAI */ YES,
  /* cstoreAO */ YES,
  /* br */       YES,
  /* cbr */      YES,
  /* cmp_LT */   YES,
  /* cmp_LE */   YES,
  /* cmp_EQ */   YES,
  /* cmp_NE */   YES,
  /* cmp_GE */   YES,
  /* cmp_GT */   YES,
  /* i2i */      YES,
  /* c2c */      YES,
  /* i2c */      YES,
  /* c2i */      YES,
  /* output */   YES,
  /* coutput */  YES,
  /* read */     YES,
  /* cread */    YES,
  /* write */    YES,
  /* cwrite */   YES,
  /* halt   */   YES
};

/* Pointer to the first instruction of the parsed block */
/* It is declared here so it is not visible to other programs that include
   the header file */
extern Instruction* first_instruction;
extern int error_found;
int yyparse();



/* Run yyparse and return a pointer to the first instruction if no
   errors occur, otherwise return NULL */
Instruction* parse()
{
    opcode_init();
    yyparse();
    if (error_found)
    {
	free_instructions(first_instruction);
	return NULL;
    }
    else
    {
      return first_instruction;
    }
}


void opcode_init(void)
{
    int i = 0;
    unsigned int new_entry_position;
    Hashnode *new_entry;

    /* Create empty table */
    hash_opcodes = (Hashnode**) malloc(HASH_SIZE*sizeof(Hashnode*));
    for(i=0; i<HASH_SIZE; i++)
	hash_opcodes[i] = NULL;

    /* Iterate through the new array of opcodes */
    i = 0;
    while (opcode_specs[i].string)
    {
	new_entry_position = hash(opcode_specs[i].string);
	new_entry = malloc(sizeof(Hashnode));
	new_entry->value = &opcode_specs[i];
	new_entry->next = hash_opcodes[new_entry_position];
	hash_opcodes[new_entry_position] = new_entry;
	i++;
    }

    /* Quickly intialize the label hash table as well */
    hash_labels = (Label**) malloc(HASH_SIZE*sizeof(Label*));
    for(i=0; i<HASH_SIZE; i++)
      hash_labels[i] = (Label *) 0;

    number_of_labels = 0;
    label_list = (Label_Array*) malloc(sizeof(Label_Array));
    for(i=0; i<LABELS_PER_ARRAY; i++)
	label_list->array[i] = NULL;
    label_list->first = 0;
    label_list->next = NULL;
}

Opcode* get_opcode(char *name)
{
    /* Select bucket corresponding to string */
    Hashnode* hash_entry = hash_opcodes[hash(name)];

    while(hash_entry && strcmp(name, hash_entry->value->string))
	hash_entry = hash_entry->next;

    if (hash_entry)
	return hash_entry->value;
    else
	return NULL;
}


/* Inserts a label into the label table and returns a numeric 
 * identifier for the label.  If the label already exists in the 
 * table the identifier is returned */
int insert_label(char* name)
{
    Label* hash_entry;
    Label_Array* current_list;
    int pos, i;
    int position;
 
    /* First check to see if the label already exists */
    position = hash(name);

    hash_entry = hash_labels[position];
    
    while(hash_entry && strcmp(name, hash_entry->string)) {
	hash_entry = hash_entry->next;
    }

    if (hash_entry)
	return hash_entry->identifier;

    /* Otherwise add to hash table */
    hash_entry = malloc(sizeof(Label));
    hash_entry->identifier = number_of_labels;
    hash_entry->string = (char*) malloc(sizeof(char)*(strlen(name)+1));
    strcpy(hash_entry->string,name);
    hash_entry->target = NULL;
    hash_entry->next = hash_labels[position];
    hash_labels[position] = hash_entry;

    /* and add to label array */
    current_list = label_list;
    while(current_list->next)
	current_list = current_list->next;
    
    if (number_of_labels > current_list->first + LABELS_PER_ARRAY - 1)
    {
	current_list->next = (Label_Array*) malloc(sizeof(Label_Array));
	for(i=0; i<LABELS_PER_ARRAY; i++)
	    current_list->next->array[i] = NULL;
	current_list->next->first = current_list->first + LABELS_PER_ARRAY;
	current_list->next->next = NULL;
	current_list = current_list->next;
    }

    pos = number_of_labels - current_list->first;
    current_list->array[pos] = hash_entry;

    return(number_of_labels++);
}


/* Returns the pointer to the label structure given the 
 * numeric identifier of the label */
Label* get_label(int index)
{
    Label_Array* current_list = label_list;

    while(index >= LABELS_PER_ARRAY)
    {
	current_list = current_list->next;
	index -= LABELS_PER_ARRAY;
    }

    return current_list->array[index];
}

/* Deallocate memory for a list of instructions */
void free_instructions(Instruction* inst)
{
    Instruction* previous;

    while(inst)
    {
	previous = inst;
	inst = inst->next;
	free_operations(previous->operations);
	free(previous);
    }
}

/* Deallocate memory for a list of operations */
void free_operations(Operation* op)
{
    Operation* previous;

    while(op)
    {
	previous = op;
	op = op->next;
	free_operands(previous->srcs);
	free_operands(previous->consts);
	free_operands(previous->labels);
	free_operands(previous->defs);
	free(previous);
    }
}

/* Deallocate memory for a list of operands */
void free_operands(Operand* op)
{
    Operand* previous;

    while(op)
    {
	previous = op;
	op = op->next;
	free(previous);
    }
}

