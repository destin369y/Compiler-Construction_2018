/*  sim.c
 *  Source code for a simulator of the ILOC subset defined in
 *  "Engineering a Compiler" by Cooper and Torczon
 *  written by Todd Waterman
 *  11/30/00 */

/* modified to add tracing and word-alignment check on load & store 
 * -- kdc, Fall 2014
 */

/* added READ and WRITE for COMP 506
 * changed the command-line protocol for handling filenames
 * -- kdc, March 2016
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "instruction.h"
#include "machine.h"
#include "sim.h"

int cycle_count;

static int first_op = 0;

static int memory_op_count;
static int mult_op_count;
static int op_count;
static int output_op_count;
static int branch_op_count;

static void complain_and_die(char *op);
static void die_quickly(int code);

static int  IntFromString(char *p, int *l);

static FILE *codefile = NULL;
static FILE *datafile = NULL;
static FILE *savedstdin = NULL;
static char filename[128];
static int DataFileName = 0;
static char backspace[4];

int main(int argc, char* argv[]) {
  int mem_size = DEFAULT_MEMORY_SIZE;
  int reg_size = DEFAULT_NUM_REGISTERS;
  int current_argument = 1;
  int machine_initialized = 0;
  int start_location;

  int temp;  /* used in string to integer conversions */
  int legal;

  int seen_r = 0;
  int seen_m = 0;
  int seen_file = 0;
  
  Instruction* code;
  char *c;

  /* Set default stall mode - branches and memory */
  set_stall_mode(3);  /* default for Lab 3 is stall mode 3 */

  tracing = 0;
  cycle_count = 0;

  backspace[0] = 0x08;
  backspace[1] = '\0';

  while(current_argument < argc) {

    c = argv[current_argument];

    if (*c != '-') {
      if (seen_file == 0) {
	seen_file = current_argument;
	codefile = fopen(argv[current_argument],"r");
	if (codefile == NULL) {
	  fprintf(stderr,"\nError: could not open file '%s' for code input.\n",
		  argv[current_argument]);
	  die_quickly(-1);
	}
      }
      else {
	fprintf(stderr,
		"\nError: invalid command-line argument ('%s').\n",
		argv[current_argument]);
	die_quickly(-1);
      }
      current_argument++;
    }
    else {
      c++;

      if (*c == 'h') {
	print_help();
	return 0;
      } 
      else if (*c == 't') {
	tracing++;
	current_argument += 1;
      }
      else if (*c == 'v') {
	fprintf(stderr,"ILOC Simulator, version %s-%d-%d.\n",
		CLASS, MAJOR_VERSION, MINOR_VERSION);
	exit(0);
      }
      else { /* All of the following flags require at least one additional 
		parameter, so perform check here. */
	if (current_argument == argc - 1) {
	  fprintf(stderr,"Invalid flag sequence: ");
	  fprintf(stderr,"make sure any required numbers are included.\n");
	  return(1);
	}
      
	if (*c == 'r') {
	  if (machine_initialized) {
	    fprintf(stderr,"\nError: -r must precede -i and -c.\n");
	    die_quickly(-1);
	  }

	  if (seen_r) {
	    fprintf(stderr,"Argument error: only one -r allowed.\n");
	    return 0;
	  }
	  else 
	    seen_r++;

	  temp  = IntFromString(argv[current_argument+1],&legal);
	  if (legal)
	    reg_size = temp;
	  else {
	    fprintf(stderr,"\nError: argument to -r ('%s') is not a valid number.\n",
		    argv[current_argument+1]);
	    fprintf(stderr,"execution halts.\n\n");
	    die_quickly(-1);
	  }
	  
	  current_argument += 2;
	}
	else if (*c == 'm') {
	  if (machine_initialized) {
	    fprintf(stderr,"\nError: -m must precede -i and -c.\n");
	    die_quickly(-1);
	  }

	  if (seen_m) {
	    fprintf(stderr,"Argument error: only one -m allowed.\n");
	    return 0;
	  }
	  else 
	    seen_m++;

	  temp  = IntFromString(argv[current_argument+1],&legal);
	  if (legal)
	    mem_size = temp;
	  else {
	    fprintf(stderr,"\nError: argument to -m ('%s') is not a valid number.\n",
		    argv[current_argument+1]);
	    fprintf(stderr,"execution halts.\n\n");
	    die_quickly(-1);
	  }
	  
	  current_argument += 2;
	}
	else if (*c == 's') {
	  temp  = IntFromString(argv[current_argument+1],&legal);
	  if (!legal) {
	    fprintf(stderr,"\nError: argument to -m ('%s') is not a valid number.\n",
		    argv[current_argument+1]);
	    fprintf(stderr,"execution halts.\n\n");
	    die_quickly(-1);
	  }
	  else if (1 > temp || temp > 3) {
	    fprintf(stderr,"\nError: stall mode must be 1, 2, or 3.\n");
	    fprintf(stderr,"Execution halts.\n\n");
	    die_quickly(-1);
	  }

	  set_stall_mode(temp);
	  current_argument += 2;
	}
	else if (*c == 'd') {
	  if (DataFileName > 0) {
	    fprintf(stderr,"\nError: multiple data options on this command line.\n");
	    fprintf(stderr,"Only one allowed.\n");
	    fprintf(stderr,"Execution halts.\n");
	    die_quickly(-1);
	  }

	  DataFileName = current_argument+1;
	  current_argument += 2;
	}
	else if (*c == 'i') {
	  start_location = IntFromString(argv[current_argument+1],&legal);
	  
	  if (!legal) {
	    fprintf(stderr,
		    "\nError: start location in -i is not a valid number ('%s')\n",
		    argv[current_argument+1]);
	    fprintf(stderr,"Execution halts.\n");
	    die_quickly(-1);
	  }
	
	  if (0 > start_location || start_location > mem_size - 1) {
	    fprintf(stderr,
		    "\nError: start address in -i option (%d) is out of range.\n",
		    start_location);
	    fprintf(stderr,"Current memory size is %d.\n\n",mem_size);
	    fprintf(stderr,"Execution halts.\n");
	    die_quickly(-1);
	  }
	  else if ((start_location % 4) != 0) {
	    fprintf(stderr,
		    "\nError: start address in -i option (%d) is not word aligned.\n",
		    start_location);
	    fprintf(stderr,"Execution halts.\n");
	    die_quickly(-1);
	  }
	  if (!machine_initialized) {
	    initialize_machine(reg_size,mem_size);
	    machine_initialized = 1;
	  }

	  current_argument += 2;
	  temp = IntFromString(argv[current_argument],&legal);

	  while(legal) {
	    set_word(start_location,temp);
	    start_location += 4;
	    current_argument++;
	    if (current_argument == argc)
	      legal = 0;
	    else
	      temp = IntFromString(argv[current_argument],&legal); 
	  }

	}
	else if (*c == 'c') {
	  char c;
	  start_location = IntFromString(argv[current_argument+1],&legal);
	  
	  if (!legal) {
	    fprintf(stderr,
		    "\nError: start location in -c is not a valid number ('%s')\n",
		    argv[current_argument+1]);
	    fprintf(stderr,"Execution halts.\n");
	    die_quickly(-1);
	  }
	
	  if (0 > start_location || start_location > mem_size - 1) {
	    fprintf(stderr,
		    "\nError: start address in -c option (%d) is out of range.\n",
		    start_location);
	    fprintf(stderr,"Current memory size is %d.\n\n",mem_size);
	    fprintf(stderr,"Execution halts.\n");
	    die_quickly(-1);
	  }

	  if (!machine_initialized) {
	    initialize_machine(reg_size,mem_size);
	    machine_initialized = 1;
	  }

	  current_argument += 2;
	  
	  fprintf(stderr,"\nMEM[%d ff] <-",start_location);

	  legal = 1;
	  while(legal) {
	    if (strlen(argv[current_argument]) == 1) {
	      c = *argv[current_argument];
	      fprintf(stderr," %c",c);
	      set_memory(start_location,c);
	      start_location++;
	      current_argument++;
	      if (current_argument == argc)
		legal = 0;
	    }
	    else
	      legal = 0;
	  }
	  fprintf(stderr,"\n\n");
	}
	else {
	  fprintf(stderr,"Invalid flag specified\n");
	  return 0;
	}
      }
    }
  }

  if (DataFileName > 0) {
    datafile = fopen(argv[DataFileName],"r");
    if (datafile == NULL) {
      fprintf(stderr,"\nError: could not open data file '%s'.\n",
	      argv[DataFileName]);
      fprintf(stderr,"Data directive fails.\n");
      die_quickly(-1);
    }
  }

  if (codefile != NULL) {
    savedstdin = stdin;
    stdin = codefile;
  }
  
  if (!machine_initialized) 
    initialize_machine(reg_size,mem_size);
  
  code = parse();
  
  if (!code) {
    fprintf(stderr,"\nError reading input file, simulator not run.\n");
    return 1;
  }
  if (codefile != NULL) {
    fclose(stdin);
  }
  
  if (datafile != NULL) {
    stdin = datafile;
  }
  else
    stdin = savedstdin;

  simulate(code);

  return 0;
};

/* Print a usage message */
void print_help()
{
    printf("Usage: sim [options] filename\n");
    printf("  Options:\n");
    printf("    -h                 display usage message\n");
    printf("    -d datafile        specifies a file as input to \"read\" ops\n");
    printf("    -m NUM             simulator has NUM bytes of memory\n");
    printf("    -r NUM             simulator has NUM available registers\n");
    printf("    -s NUM             simulator stalls for the following conditions:\n");
    printf("                         0:  nothing\n");
    printf("                         1:  branches\n");
    printf("                         2:  branches and memory interlocks\n");
    printf("                         3:  branches and both register and memory interlocks\n");
    printf("                         default setting is -s 3\n");
    printf("    -t                 print a trace of simulator execution\n");
    printf("    -i NUM ... NUM     starting at the memory location specified by the first\n");
    printf("                         NUM put the remaining NUMs into memory as words.\n");
    printf("                         Must be the last option specified\n");
    printf("    -c NUM ... NUM     same as -i, but puts the NUMs into memory as bytes\n");
    printf("    -v                 prints the simulator's version number.\n\n");
    printf("  filename must be a valid ILOC input file.\n\n");
    printf("  If no \"-d\" file is specified, \"read\" ops default\n");
    printf("  to stdin.\n");
    printf("  See the docs for more detail.\n\n");
    printf("  Note: any use of -r or -m must precede any use of -i or -c.\n");
    printf("  The -i and -c options initialize the simulator's memory.\n"); 
    printf("  After that point, -r and -m no longer have an effect.\n");
}

/* Set stall flags appropriately */
void set_stall_mode(int mode)
{
    stall_on_branches = 0;
    stall_on_memory = 0;
    stall_on_registers = 0;

    switch(mode)
    {
	case 3:
	  stall_on_registers = 1;
	case 2:
	  stall_on_memory = 1;
	case 1:
	  stall_on_branches = 1;
	case 0:
	  break;
	default:
	  fprintf(stderr,"Illegal safety mode specified.\n");
	  die_quickly(-1);
      }
}


/* Simulate the code and output results to standard out */
void simulate(Instruction* code)
{
    Change* list_of_effects = NULL; 
    Change* last_effect = NULL;
    Change* new_effects;
    int instruction_count = 0;
    int operation_count = 0;

    if (tracing) {
      fprintf(stdout, "ILOC Simulator, Version %s-%d-%d\n",
	      CLASS,MAJOR_VERSION,MINOR_VERSION);
      fprintf(stdout,"Interlock settings: ");
      if (stall_on_memory)
	fprintf(stdout,"memory ");
      if (stall_on_registers)
	fprintf(stdout,"registers ");
      if (stall_on_branches)
	fprintf(stdout,"branches ");
      if (! (stall_on_memory || stall_on_registers || stall_on_branches))
	fprintf(stdout,"no hardware stalls ");
      fprintf(stdout,"\n\n");
    }

    while(code && !halt_now)
    {
        if (tracing) {
	  fprintf(stdout,"%d:\t[",cycle_count);
	  first_op = 1;
	}

	if (!((memory_stall(code,list_of_effects) && stall_on_memory) ||
	      (register_stall(code,list_of_effects) && stall_on_registers) ||
	      (branch_stall(list_of_effects) && stall_on_branches) ||
	      (antidependence_stall(code,list_of_effects) && stall_on_registers)))
	{
	    memory_op_count = 0;
	    mult_op_count = 0;
	    op_count = 0;
	    output_op_count = 0;
	    branch_op_count = 0;
         
	    new_effects = execute_instruction(code,&operation_count);

	    /* checks restrictions on FU use */
#ifdef LAB3
	    if (op_count > 2 || memory_op_count > 1 || mult_op_count > 1 ||
		output_op_count > 1 || branch_op_count > 1 ) {
#endif
#ifdef LAB1
           if (op_count > 1) {
#endif
	        fprintf(stderr,"\nError: Machine constraints violated in cycle %d.\n\n",
			cycle_count);
		if (tracing)
		  fprintf(stdout,"]\n");
		else
		  fprintf(stdout,"\n");
		fflush(stdout);
		die_quickly(-1);
	    }
		
	    instruction_count++;

	    if (!list_of_effects)
	    {
		list_of_effects = new_effects;
		last_effect = new_effects;
	    }
	    else
		last_effect->next = new_effects;

	    /* Move last effect to end */
	    if (last_effect)
		while(last_effect->next)
		    last_effect = last_effect->next;

	    /* Go to next instruction */
	    code = code->next;
	}
        else if (tracing)
	  fprintf(stdout," stall ");
	
        if (tracing)
	  fprintf(stdout,"]");
	list_of_effects = execute_changes(list_of_effects,&last_effect,&code);
	cycle_count++;
	if (tracing)
	  fprintf(stdout,"\n");

    }

    while(list_of_effects)
    {
      if (tracing) {
	fprintf(stdout,"%d:\t[stall]",cycle_count);
      }
      list_of_effects = execute_changes(list_of_effects,&last_effect,&code);
      cycle_count++;
      if (tracing)
	fprintf(stdout,"\n");	
    }

    fprintf(stdout,"\nExecuted %d instructions and %d operations in %d cycles.\n",
	    instruction_count,operation_count,cycle_count);

}

/* Returns 1 if the instruction uses a register that is not ready */
int register_stall(Instruction* inst,Change* changes)
{
    Operation* current_op = inst->operations;

    while(current_op)
    {
	/* Check source registers for operation */
	if (!list_of_operands_ready(current_op->srcs,changes))
	    return 1;

	/* Also check target registers on stores */
	if ((opcode_specs[current_op->opcode].target_is_source) &&
	    (!list_of_operands_ready(current_op->defs,changes)))
	    return 1;

	current_op = current_op->next;
    }

    /* All registers are ready if this point is reached */
    return 0;
}

/* Returns 1 if some register defined in the instruction is */
/* already pending in the effects list 'changes' */
/* 2014 Fix applied by KDC */
int antidependence_stall(Instruction *inst, Change *changes) {

  Operation *op = inst->operations;
  Operand   *def;
  Change    *c;

  while(op) {
    def = op->defs;
    while(def) {
      c = changes;
      while(c) {
        if (c->type == REGISTER && c->location == def->value)
	  return 1;
	c = c->next;
      }
      def = def->next;
    }
    op  = op->next;
  }

  return 0;
}

/* Returns 1 if all operands in the list are ready, and 
   return 0 if they are not */
int list_of_operands_ready(Operand* reg, Change* changes)
{
    while(reg)
    {
	/* Non zero values in register_ready indicate that
	   the register is not ready */
	if (!reg_ready(reg->value,changes))
	    return 0;
	reg = reg->next;
    }
    
    /* All register are ready if this point is reached */
    return 1;
}

/* Returns 1 if the instruction uses a memory address that is not ready */
int memory_stall(Instruction* inst, Change* changes)
{
    int memory_location;
    Operation* current_op = inst->operations;

    while(current_op)
    {
	switch(current_op->opcode)
	{
	    case LOAD:
	    memory_location = get_register(current_op->srcs->value);
	    if (!word_ready(memory_location,changes))
		return 1;
	    break;

	    case LOADAI:
	    memory_location = get_register(current_op->srcs->value) +
		current_op->consts->value;
	    if (!word_ready(memory_location,changes))
		return 1;
	    break;
	    
	    case LOADAO:
	    memory_location = get_register(current_op->srcs->value) +
		get_register(current_op->srcs->next->value);
	    if (!word_ready(memory_location,changes))
		return 1;
	    break;
	    
	    case CLOAD:
	    memory_location = get_register(current_op->srcs->value);
	    if (!mem_ready(memory_location,changes))
		return 1;
	    break;
	    
	    case CLOADAI:
	    memory_location = get_register(current_op->srcs->value) +
		current_op->consts->value;
	    if (!mem_ready(memory_location,changes))
		return 1;
	    break;

	    case CLOADAO:
	    memory_location = get_register(current_op->srcs->value) +
		get_register(current_op->srcs->next->value);
	    if (!mem_ready(memory_location,changes))
		return 1;
	    break;

	    case OUTPUT:
	    memory_location = current_op->consts->value;
	    if (!mem_ready(memory_location,changes)) /* mem_ready or word_ready ? */
		return 1;
	    break;


	    default:
	    break;
	
	}

	current_op = current_op->next;
    }
    /* All memory locations are ready if this point is reached */
    return 0;
}

/* Returns 1 if the register does not depend on the list of effects */
int reg_ready(int reg, Change* effects)
{
    while(effects)
    {
	if (effects->type == REGISTER && effects->location == reg)
	    return 0;
	effects = effects->next;
    }
    return 1;
}

/* Returns 1 if the memory location does not depend on the list of effects */
int mem_ready(int location, Change* effects)
{
    while(effects)
    {
	if (effects->type == MEMORY && effects->location == location)
	    return 0;
	effects = effects->next;
    }
    return 1;
}

/* Returns 1 if the word of memory does not depend on the list of effects */
int word_ready(int location, Change* effects)
{
    while(effects)
    {
	if (effects->type == MEMORY && effects->location >= location
	    && effects->location <= (location+3))
	    return 0;
	effects = effects->next;
    }
    return 1;
}

/* Execute all operations in a single instruction */
Change* execute_instruction(Instruction* inst, int* op_count)
{
    Operation* current_op = inst->operations;
    Change* all_changes = NULL;
    Change* last_change;
    Change* new_changes;

    while(current_op)
    {
	(*op_count)++;
	new_changes = execute_operation(current_op);
	
	if (!all_changes)
	{
	    all_changes = new_changes;
	    last_change = new_changes;
	}
	else
	    last_change->next = new_changes;

	/* Move last change to end */
	if (last_change)
	    while(last_change->next)
		last_change = last_change->next;
		
	current_op = current_op->next;
    }

    return(all_changes);
}

/* tracing routines -- kdc */

/* Next steps:
 * - expand trace operation to show source registers and target reg & value
 * - show register number or memory location in a delayed effect
 * - handle semicolons correctly
 * - think about a bulk initialization for RAM
 * 
 * 
 * 
 */
static char tr_buf[16], tr_string[256];
static int last_traced_op = -1;
static int last_traced_effect = -1;

static char *StringFromInt( int num ) {
  sprintf(tr_buf,"%d",num);
  return tr_buf;
}

static void trace_operation( char * name, char *value ) {
  if (tracing) {
    if (last_traced_op != cycle_count) {
      last_traced_op = cycle_count;
      last_traced_effect = -1;
    }

    if (first_op == 1) {
      fprintf(stdout,"%s %s",name,value);
      first_op = 0;
    }
    else 
      fprintf(stdout,"; %s %s",name,value);
  }
}

static void trace_effect( int issued ) {
  if (tracing) {
    if (last_traced_effect != issued) 
      fprintf(stdout," *%d",issued);

    last_traced_effect = issued;
  }
}

static char *immed_string( int c1, int r2, int value ) {
  sprintf(tr_string,"%d => r%d (%d)",c1,r2,value);
  return tr_string;
}

static char *addi_string( int r1, int r2, int r3, int value ) {
  sprintf(tr_string,"r%d (%d), %d => r%d (%d)",r1,get_register(r1),r2,r3,value);
  return tr_string;
}

static char *store_string( int r1, int r2, int value ) {
  sprintf(tr_string,"r%d (%d) => r%d (addr: %d)",
	  r1, get_register(r1), r2, get_register(r2));
  return tr_string;
}

static char *load_string( int r1, int r2, int value ) {
  sprintf(tr_string,"r%d (addr: %d) => r%d (%d)",r1,get_register(r1),r2,value);
  return tr_string;
}

static char *one_op_string( int r1, int r2, int value ) {
  sprintf(tr_string,"r%d (%d) => r%d (%d)", r1, get_register(r1), r2, value);
  return tr_string;
}

static char *two_op_string( int r1, int r2, int r3, int value ) {
  sprintf(tr_string,"r%d (%d), r%d (%d) => r%d (%d)",
	  r1, get_register(r1),
	  r2, get_register(r2), 
	  r3,value);
  return tr_string;
}

static char *output_string( int c1, int val ) {
  sprintf(tr_string,"%d (%d)",c1,val);
  return tr_string;
}

static char *br_string( char *l1 ) {
  sprintf(tr_string,"-> %s",l1);
  return tr_string;
}

static char *cbr_string( int r1, char *l1, char *l2, int value ) {
  if (value == 1)
    sprintf(tr_string,"r%d (%d) -> %s*, %s",r1,get_register(r1),l1,l2);
  else 
    sprintf(tr_string,"r%d (%d) -> %s, %s*",r1,get_register(r1),l1,l2);
  return tr_string;
}

static char *i2i_string( int r1, int r2, int v2 ){
  sprintf(tr_string,"r%d (%d) => r%d (%d)",r1,get_register(r1),r2,v2);
  return tr_string;
}

static char char_buf0[16], char_buf1[16];
 
static char* CharOfValue( int v, char *buffer ) {
  if (v < 0 || 255 < v) {
    fprintf(stderr,"CharOfValue( %d ): value out of range.\n",v);
  }
  sprintf(buffer,"%c",(char)v);
  return buffer;
}
 
static char *copy_string( int r1, char *v1, int r2, char *v2 ) {
  sprintf(tr_string,"r%d (%s) => r%d (%s)",r1,v1,r2,v2);
  return tr_string;
}
 
 static char *read_string( int r1, int v1 ){
   sprintf(tr_string,"(%d) => r%d",v1,r1);
   return tr_string;
 }

  static char *cread_string( int r1, int v1 ){
    sprintf(tr_string,"(%c) => r%d",(char) v1,r1);
   return tr_string;
 }

 static char *write_string( int r1, int v1 ) {
   sprintf(tr_string,"r%d (%d)",r1,v1);
   return tr_string;
 }

  static char *cwrite_string( int r1, int v1 ) {
    sprintf(tr_string,"r%d (%c)",r1,(char) v1);
   return tr_string;
 }

/* Execute a single operation */
Change* execute_operation(Operation* op)
{
    Change* effects;
    Change* effect_ptr;
    int i;
    int address;
    int val;
    char cval;

    op_count++;

    if (permitted_opcode[op->opcode] == 0) {
      fprintf(stderr,"\nSimulator opcode violation.\n");
      fprintf(stderr,
	      "Opcode '%s' is not supported in this version.\n",
	      opcode_specs[op->opcode].string);
      die_quickly(-1);
    }

    if (halt_now == 1) {
      trace_operation("nop","(after halt)");
      return NULL;
    }

    /* This is a big, ugly switch statement that deals with every operation */
    switch(op->opcode)
    {
	case NOP:
	  trace_operation("nop",backspace);
	  return NULL;
	  break;

        case HALT:
	  trace_operation("halt","(after committing effects)");
	  /* global */ halt_now = 1;
	  return NULL;
	  break;

        case ADD:
	  effects = onereg(op);
	  effects->value = get_register(op->srcs->value) + 
	      get_register(op->srcs->next->value);
	  trace_operation("add",two_op_string(op->srcs->value,op->srcs->next->value,
					      op->defs->value, effects->value));
	  return effects;
	  break;

	case SUB:
	  effects = onereg(op);
	  effects->value = get_register(op->srcs->value) - 
	      get_register(op->srcs->next->value);
	  trace_operation("sub",two_op_string(op->srcs->value,op->srcs->next->value,
					      op->defs->value, effects->value));
	  return effects;
	  break;

	case MULT:
	  effects = onereg(op);
	  effects->value = get_register(op->srcs->value) * 
	      get_register(op->srcs->next->value);
	  trace_operation("mult",
			  two_op_string(op->srcs->value,op->srcs->next->value,
					op->defs->value, effects->value));
	  mult_op_count++;
	  return effects;
	  break;

	case DIV:
	  effects = onereg(op);
	  effects->value = get_register(op->srcs->value) / 
	      get_register(op->srcs->next->value);
	  trace_operation("div",two_op_string(op->srcs->value,op->srcs->next->value,
					      op->defs->value, effects->value));
	  return effects;
	  break;	  

	case ADDI:
	  effects = onereg(op);
	  effects->value = get_register(op->srcs->value) + op->consts->value;
	  trace_operation("addI",addi_string(op->srcs->value,op->consts->value,
					     op->defs->value,effects->value));
	  return effects;
	  break;

	case SUBI:
	  effects = onereg(op);
	  effects->value = get_register(op->srcs->value) - op->consts->value;
	  trace_operation("subI",addi_string(op->srcs->value,op->consts->value,
					     op->defs->value,effects->value));
	  return effects;
	  break;
	  
	case MULTI:
	  effects = onereg(op);
	  effects->value = get_register(op->srcs->value) * op->consts->value;
	  trace_operation("multI",addi_string(op->srcs->value,op->consts->value,
					     op->defs->value,effects->value));
	  return effects;
	  break;

	case DIVI:
	  effects = onereg(op);
	  effects->value = get_register(op->srcs->value) / op->consts->value;
	  trace_operation("divI",addi_string(op->srcs->value,op->consts->value,
					     op->defs->value,effects->value));
	  return effects;
	  break;

	case LSHIFT:
	  effects = onereg(op);
	  effects->value = get_register(op->srcs->value) << 
	      get_register(op->srcs->next->value);
	  trace_operation("lshift",two_op_string(op->srcs->value,op->srcs->next->value,
					      op->defs->value, effects->value));
	  return effects;
	  break;

	case LSHIFTI:
	  effects = onereg(op);
	  effects->value = get_register(op->srcs->value) << op->consts->value;
	  trace_operation("lshiftI",addi_string(op->srcs->value,op->consts->value,
					     op->defs->value,effects->value));
	  return effects;
	  break;

	case RSHIFT:
	  effects = onereg(op);
	  effects->value = get_register(op->srcs->value) >> 
	      get_register(op->srcs->next->value);
	  trace_operation("rshift",two_op_string(op->srcs->value,op->srcs->next->value,
					      op->defs->value, effects->value));
	  return effects;
	  break;

	case RSHIFTI:
	  effects = onereg(op);
	  effects->value = get_register(op->srcs->value) >> op->consts->value;
	  trace_operation("lshiftI",addi_string(op->srcs->value,op->consts->value,
					     op->defs->value,effects->value));
	  return effects;
	  break;

        case AND:
	  effects = onereg(op);
	  effects->value = get_register(op->srcs->value) && 
	      get_register(op->srcs->next->value);
	  trace_operation("and",two_op_string(op->srcs->value,op->srcs->next->value,
					      op->defs->value, effects->value));
	  return effects;
	  break;

        case ANDI:
	  effects = onereg(op);
	  effects->value = get_register(op->srcs->value) && op->consts->value;
	  trace_operation("andI",addi_string(op->srcs->value,op->consts->value,
					     op->defs->value,effects->value));
	  return effects;
	  break;

        case OR:
	  effects = onereg(op);
	  effects->value = get_register(op->srcs->value) || 
	      get_register(op->srcs->next->value);
	  trace_operation("or",two_op_string(op->srcs->value,op->srcs->next->value,
					      op->defs->value, effects->value));
	  return effects;
	  break;

        case ORI:
	  effects = onereg(op);
	  effects->value = get_register(op->srcs->value) || op->consts->value;
	  trace_operation("orI",addi_string(op->srcs->value,op->consts->value,
					     op->defs->value,effects->value));
	  return effects;
	  break;

        case NOT:
	  effects = onereg(op);
	  effects->value = get_register(op->srcs->value);
	  if (effects->value)
	    effects->value = 0;
          else 
	    effects->value = 1;
	  trace_operation("not",one_op_string(op->srcs->value,op->defs->value,
					       effects->value));
	  return effects;
	  break;

	case LOADI:
	  effects = onereg(op);
	  effects->value = op->consts->value;
	  trace_operation("loadI",immed_string(op->consts->value,op->defs->value,
					       effects->value));
	  return effects;
	  break;

	case LOAD:
	  effects = onereg(op);
	  address = get_register(op->srcs->value);
	  if ((address % 4) != 0) 
	    complain_and_die("load");

	  effects->value = get_word(address);
	  trace_operation("load",load_string(op->srcs->value,op->defs->value,
	      effects->value));
	  memory_op_count++;
	  return effects;
	  break;

	case LOADAI:
	  effects = onereg(op);
	  address = get_register(op->srcs->value) + op->consts->value;
	  if ((address % 4) != 0)
	    complain_and_die("loadAI");

	  effects->value = get_word(address);
	  trace_operation("loadAI",addi_string(op->srcs->value,op->consts->value,
					     op->defs->value,effects->value));
	  memory_op_count++;
	  return effects;
	  break;

	case LOADAO:
	  effects = onereg(op);
	  address = get_register(op->srcs->value) 
	          + get_register(op->srcs->next->value);
	  if ((address % 4) != 0)
	    complain_and_die("loadAO");

	  effects->value = get_word(get_register(op->srcs->value) +
				    get_register(op->srcs->next->value));
	  trace_operation("loadAO",
			  two_op_string(op->srcs->value,op->srcs->next->value,
					op->defs->value, effects->value));
	  memory_op_count++;
	  return effects;
	  break;

	case CLOAD:
	  effects = onereg(op);
	  effects->value = get_memory(get_register(op->srcs->value));
	  trace_operation("cload",load_string(op->srcs->value,op->defs->value,
	      effects->value));
	  memory_op_count++;
	  return effects;
	  break;

	case CLOADAI:
	  effects = onereg(op);
	  effects->value = get_memory(get_register(op->srcs->value) +
				      op->consts->value);
	  trace_operation("cloadAI",
			  addi_string(op->srcs->value,op->consts->value,
				      op->defs->value,effects->value));
	  memory_op_count++;
	  return effects;
	  break;

	case CLOADAO:
	  effects = onereg(op);
	  effects->value = get_memory(get_register(op->srcs->value) +
				      get_register(op->srcs->next->value));
	  trace_operation("cloadAO",StringFromInt(effects->value));
	  memory_op_count++;
	  return effects;
	  break;
	  
	case STORE:
	  effect_ptr = NULL;
	  address = get_register(op->defs->value);
	  if ((address % 4) != 0)
	    complain_and_die("store");

	  for(i=0;i<4;i++)
	  {
	      effects = storeop(op);
	      effects->value = (get_register(op->srcs->value) << (8*i)) >> 24;
	      effects->location = get_register(op->defs->value) + i;
	      effects->next = effect_ptr;
	      effect_ptr = effects;
	  }
	  trace_operation("store",store_string(op->srcs->value,op->defs->value,
					       get_register(op->srcs->value)));
	  memory_op_count++;
	  return effects;
	  break;

	case STOREAI:
	  effect_ptr = NULL;
          address = get_register(op->defs->value) + op->consts->value;
	  if ((address % 4) != 0)
	    complain_and_die("storeAI");

	  for(i=0;i<4;i++)
	  {
	      effects = storeop(op);
	      effects->value = (get_register(op->srcs->value) << (8*i)) >> 24;
	      effects->location = get_register(op->defs->value) + 
		  op->consts->value + i;
	      effects->next = effect_ptr;
	      effect_ptr = effects;
	  }
	  trace_operation("storeAI",addi_string(op->srcs->value,op->consts->value,
					     op->defs->value,effects->value));
	  memory_op_count++;
	  return effects;
	  break;

        case STOREAO:
	  effect_ptr = NULL;
          address = get_register(op->defs->value) 
	      + get_register(op->defs->next->value);
	  if ((address % 4) != 0)
	    fprintf(stderr,
		    "storeAO operation at %d attempts non-aligned access.\n",
		    cycle_count);	   

	  for(i=0;i<4;i++)
	  {
	      effects = storeop(op);
	      effects->value = (get_register(op->srcs->value) << (8*i)) >> 24;
	      effects->location = get_register(op->defs->value) + 
		  get_register(op->defs->next->value) + i;
	      effects->next = effect_ptr;
	      effect_ptr = effects;
	  }
	  trace_operation("div",
			  two_op_string(op->srcs->value,op->srcs->next->value,
					      op->defs->value, effects->value));
	  memory_op_count++;
	  return effects;
	  break;

	case CSTORE:
	  effects = storeop(op);
	  effects->value = (get_register(op->srcs->value) << 24) >> 24;
	  effects->location = get_register(op->defs->value);
	  effects->next = NULL;
	  trace_operation("cstore",store_string(op->srcs->value,op->defs->value,
						effects->value));
	  memory_op_count++;
	  return effects;
	  break;

	case CSTOREAI:
	  effects = storeop(op);
	  effects->value = (get_register(op->srcs->value) << 24) >> 24;
	  effects->location = get_register(op->defs->value) + op->consts->value;
	  effects->next = NULL;
	  trace_operation("cstoreAI",addi_string(op->srcs->value,op->consts->value,
					     op->defs->value,effects->value));
	  memory_op_count++;
	  return effects;
	  break;

	case CSTOREAO:
	  effects = storeop(op);
	  effects->value = (get_register(op->srcs->value) << 24) >> 24;
	  effects->location = get_register(op->defs->value) + 
	      get_register(op->defs->next->value);
	  effects->next = NULL;
	  trace_operation("cstoreAO",
			  two_op_string(op->srcs->value,op->srcs->next->value,
					op->defs->value, effects->value));
	  memory_op_count++;
	  return effects;
	  break;

	case BR:
	  effects = (Change*)malloc(sizeof(Change));
	  effects->type = BRANCH;
	  effects->target = (get_label(op->labels->value))->target;
	  effects->cycles_away = opcode_specs[BR].latency;
	  effects->next = NULL;
	  effects->issue_cycle = cycle_count;
	  trace_operation("br",br_string((get_label(op->labels->value))->string));
	  branch_op_count++;
	  return effects;
	  break;

	case CBR:
	  effects = (Change*)malloc(sizeof(Change));
	  effects->type = BRANCH;
	  if (get_register(op->srcs->value)) {
	    effects->target = (get_label(op->labels->value))->target;
	  }
	  else {
	    effects->target = (get_label(op->labels->next->value))->target;
	  }
	  effects->cycles_away = opcode_specs[BR].latency;
	  effects->next = NULL;
	  effects->issue_cycle = cycle_count;
	  trace_operation("cbr",
			  cbr_string(op->srcs->value,
				     (get_label(op->labels->value))->string,
				    (get_label(op->labels->next->value))->string,
				     get_register(op->srcs->value)));
	  branch_op_count++;
	  return effects;
	  break;

	case CMPLT:
	  effects = onereg(op);
	  if (get_register(op->srcs->value) < 
	      get_register(op->srcs->next->value))
	      effects->value = 1;
	  else
	      effects->value = 0;

	  trace_operation("cmp_LT",two_op_string(op->srcs->value,
						 op->srcs->next->value,
						 op->defs->value,
						 effects->value));
	  return effects;
	  break;
	  
	case CMPLE:
	  effects = onereg(op);
	  if (get_register(op->srcs->value) <= 
	      get_register(op->srcs->next->value))
	      effects->value = 1;
	  else
	      effects->value = 0;

	  trace_operation("cmp_LE",two_op_string(op->srcs->value,
						 op->srcs->next->value,
						 op->defs->value,
						 effects->value));
	  return effects;
	  break;

	case CMPEQ:
	  effects = onereg(op);
	  if (get_register(op->srcs->value) == 
	      get_register(op->srcs->next->value))
	      effects->value = 1;
	  else
	      effects->value = 0;

	  trace_operation("cmp_EQ",two_op_string(op->srcs->value,
						 op->srcs->next->value,
						 op->defs->value,
						 effects->value));
	  return effects;
	  break;

	case CMPNE:
	  effects = onereg(op);
	  if (get_register(op->srcs->value) != 
	      get_register(op->srcs->next->value))
	      effects->value = 1;
	  else
	      effects->value = 0;

	  trace_operation("cmp_NE",two_op_string(op->srcs->value,
						 op->srcs->next->value,
						 op->defs->value,
						 effects->value));
	  return effects;
	  break;

	case CMPGE:
	  effects = onereg(op);
	  if (get_register(op->srcs->value) >= 
	      get_register(op->srcs->next->value))
	      effects->value = 1;
	  else
	      effects->value = 0;

	  trace_operation("cmp_GE",two_op_string(op->srcs->value,
						 op->srcs->next->value,
						 op->defs->value,
						 effects->value));
	  return effects;
	  break;

	case CMPGT:
	  effects = onereg(op);
	  if (get_register(op->srcs->value) > 
	      get_register(op->srcs->next->value))
	      effects->value = 1;
	  else
	      effects->value = 0;

	  trace_operation("cmp_GT",two_op_string(op->srcs->value,
						 op->srcs->next->value,
						 op->defs->value,
						 effects->value));
	  return effects;
	  break;

	case I2I:
	  effects = onereg(op);
	  effects->value = get_register(op->srcs->value);
	  trace_operation("i2i",i2i_string(op->srcs->value,
					   op->defs->value,
					   effects->value));
	  return effects;
	  break;

	case C2C:
	  effects = onereg(op);
	  effects->value = (get_register(op->srcs->value) << 24) >> 24;
	  trace_operation("c2c",
			  copy_string(op->srcs->value,
				      CharOfValue(effects->value,char_buf0),
				      op->defs->value,
				      CharOfValue(effects->value,char_buf1)));
	  return effects;
	  break;

        case C2I:
	  effects = onereg(op);
	  effects->value = (get_register(op->srcs->value) << 24) >> 24;
	  trace_operation("c2i",
			  copy_string(op->srcs->value,
				      CharOfValue(effects->value,char_buf0),
				      op->defs->value,
				      StringFromInt(effects->value)));
	  return effects;
	  break;
	  
	case I2C:
	  effects = onereg(op);
	  effects->value = (get_register(op->srcs->value) << 24) >> 24;
	  trace_operation("i2c",
			  copy_string(op->srcs->value,
				      StringFromInt(effects->value),
				      op->defs->value,
				      CharOfValue(effects->value,char_buf1)));
	  return effects;
	  break;
	  
	case OUTPUT:
	  effects = (Change*)malloc(sizeof(Change));
	  effects->type = DISPLAY;
	  effects->cycles_away = opcode_specs[OUTPUT].latency;
	  effects->next = NULL;
	  effects->value = get_word(op->consts->value);
	  effects->issue_cycle = cycle_count;
	  trace_operation("output",
			  output_string(op->consts->value,effects->value));
	  output_op_count++;
	  return effects;
	  break;
	  
	case COUTPUT:
	  effects = (Change*)malloc(sizeof(Change));
	  effects->type = DISPLAY;
	  effects->cycles_away = opcode_specs[OUTPUT].latency;
	  effects->next = NULL;
	  effects->value = get_memory(op->consts->value);
	  effects->issue_cycle = cycle_count;
	  trace_operation("coutput",
			  output_string(op->consts->value,effects->value));
	  output_op_count++;
	  return effects;
	  break;

        case CREAD: 
	  effects = onereg(op);

          if (stdin == savedstdin) 
	    fprintf(stdout,"Enter single character: ");

	    val = fscanf(stdin,"%c",&cval);

	    if (cval == '\n')
	      val = fscanf(stdin,"%c",&cval);
	    fprintf(stdout,"Simulator read character %d.\nfscanf() returned %d\n",
		    (int)cval,val);
 
          effects->value = (int)cval;
          trace_operation("cread", cread_string(op->defs->value,cval));

	  return effects;
	  break;

       case READ: 
	  effects = onereg(op);

          if (stdin == savedstdin)
	    fprintf(stdout,"Enter number: ");
	  fscanf(stdin,"%d",&val);
	  
          effects->value = val;
          trace_operation("read", read_string(op->defs->value,val));

	  return effects;
	  break;

        case WRITE: 
	  effects = (Change*)malloc(sizeof(Change));
	  effects->type = IOWRITE;
	  effects->cycles_away = opcode_specs[OUTPUT].latency;
	  effects->next = NULL;
	  effects->value = get_register(op->srcs->value);
	  effects->issue_cycle = cycle_count;
	  trace_operation("write",
			  write_string(op->srcs->value,effects->value));
	  output_op_count++;
	  return effects;
	  break;

         case CWRITE: 
	  effects = (Change*)malloc(sizeof(Change));
	  effects->type = IOCWRITE;
	  effects->cycles_away = opcode_specs[OUTPUT].latency;
	  effects->next = NULL;
	  effects->value = get_register(op->srcs->value);
	  effects->issue_cycle = cycle_count;
	  trace_operation("cwrite",
			  cwrite_string(op->srcs->value,effects->value));
	  output_op_count++;
	  return effects;
	  break;

    default:
	  fprintf(stderr,"Simulator Error: Invalid opcode encountered in execute_operation.");
	  return NULL;
	  break;
      }
}

/* onereg creates most of a change structure for the common case where 
   a single register is defined. */
Change* onereg(Operation* op)
{
    Change* effect = (Change*)malloc(sizeof(Change));
    effect->type = REGISTER;
    effect->location = op->defs->value;
    effect->cycles_away = opcode_specs[op->opcode].latency;
    effect->next = NULL;
    effect->issue_cycle = cycle_count;
    return effect;
}

/* storeop creates most of a change structure for a store operation */
Change* storeop(Operation* op)
{
    Change* effect = (Change*)malloc(sizeof(Change));
    effect->type = MEMORY;
    effect->cycles_away = opcode_specs[op->opcode].latency;
    effect->next = NULL;
    effect->issue_cycle = cycle_count;
    return effect;
}




/* Returns 1 if there is an outstanding branch instruction */
int branch_stall(Change* changes)
{
    while(changes)
    {
	if (changes->type == BRANCH)
	    return 1;
	changes = changes->next;
    }
    return 0;
}

/* Reduces the cycles_away of all changes by one and executes any changes
   that have a cycles_away of 0 */
Change* execute_changes(Change* changes, Change** last_change_ptr, Instruction** code_ptr)
{
    Change* first_change = changes;
    Change* prev_change = NULL;

    /* Iterate through all changes */
    while(changes)
    {
	changes->cycles_away -= 1;
	
	if (changes->cycles_away == 0)
	{
	    /* Perform effect */
	    switch(changes->type)
	    {
		case REGISTER:
		  set_register(changes->location,changes->value);
		  break;
		case MEMORY:
		  set_memory(changes->location,changes->value);
		  break;
		case BRANCH:
		  *code_ptr = changes->target;
		  break;
		case DISPLAY:
		  if (tracing)
		    fprintf(stdout,"\noutput generates => %d",
			    changes->value);
		  else
		    fprintf(stdout,"%d\n",changes->value);
		  break;
	        case IOWRITE:
		  if (tracing)
		    fprintf(stdout,"\nwrite generates => %d",
			    changes->value);
		  else
		    fprintf(stdout,"%d\n",changes->value);
		  break;
	        case IOCWRITE:
		  if (tracing)
		    fprintf(stdout,"\ncwrite generates => %c",
			    (char) changes->value);
		  else
		    fprintf(stdout,"%c\n",(char)changes->value);
		  break;		  
	      }

	    /* report delayed effect */
	    if (tracing && changes->issue_cycle != cycle_count)
	      trace_effect(changes->issue_cycle);

	    /* Delete change record */
	    if (prev_change)
	    {
		prev_change->next = changes->next;
		free(changes);
		changes = prev_change->next;
	    }
	    else
	    {
		first_change = changes->next;
		free(changes);
		changes = first_change;
	    }
	}
	else
	{
	    prev_change = changes;
	    changes = changes->next;
	}

    }

    *last_change_ptr = prev_change;
    return (first_change);
}


void complain_and_die(char *op) {
  fflush(stdout);
  fprintf(stderr,
	  "\nError: operation '%s' in cycle %d attempted a non-aligned access.\n",
	  op, cycle_count);
  fprintf(stderr,"Execution halts.\n\n");
  exit(-1);
}

void die_quickly(int code) {
  fclose(stdin);
  if (DataFileName > 0)
    (void) remove(filename);
  exit(code);
}

int IntFromString(char *p, int *legal) {
  int value;
  char *endptr;

  value = (int) strtol(p,&endptr,10);
  if (*p != '\0' && *endptr == '\0')
    *legal = 1;
  else 
    *legal = 0;

  return value;
}


