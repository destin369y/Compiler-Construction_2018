/* iloc.y
 * Yacc specification for the ILOC subset defined in
 * "Engineering a Compiler" by Cooper and Torczon
 * written by Todd Waterman
 * 11/30/00 */

/* modification
 * Changed instruction_list productions to use left 
 * recursion rather than right recursion. Fix was 
 * done to allow the simulator to parse files with 
 * more than 8,000 lines of code.  
 * 
 * It was blowing out on the size of the parse stack.
 * See Section 3.5.4 (pages 144ff) in EaC2e.
 * -- keith
 */

%{
  #define YYERROR_VERBOSE 

  #include <stdio.h>
  #include <string.h>
  #include <stdlib.h>
  #include "instruction.h"
  #include "machine.h"

  extern int yylex(void);
  char token[256];

  #define MAX_ERROR_MESSAGE_LENGTH 100

  Operands* new_operands(void);
  Operand* append_operands(Operand*,Operand*);
  int verify_args(Opcode*,int,int,int,int);

  extern char yytext[];

  extern int line_counter;
  extern Opcode* current_opcode;

  /* This function must be defined */
  void yyerror(char*);

  /* If an error is encountered during parsing this is changed to 1 */
  int error_found = 0;

  /* Pointer to the first instruction */
  Instruction* first_instruction;

  /* Pointer to the end of the instruction list */
  Instruction* EndOfList;

  /* address for data initializations */
  int Start;

%}

%union {
  int ival;
  char cval;
  Instruction* inst_ptr;
  Operation* op_ptr;
  Operands* operands_ptr;
  Operand* operand_ptr;
  Opcode* opcode_ptr;
}

%token OPEN_BRACKET
%token CLOSE_BRACKET
%token SEMICOLON
%token COMMA
%token ARROW
%token OPCODE
%token DOUTPUT
%token REGISTER
%token NUMBER
%token LABEL
%token TARGET

%token DATA_CHAR
%token DATA_INT

%type <inst_ptr> instruction_list
%type <inst_ptr> instruction
%type <op_ptr> operation_list
%type <op_ptr> operation
%type <opcode_ptr> the_opcode
%type <operands_ptr> operand_list
%type <operand_ptr> reg
%type <operand_ptr> const
%type <operand_ptr> lbl
%type <ival> label_def

%type <ival> data_defs
%type <ival> data_def
%type <ival> addr
%type <ival> listofchar
%type <ival> listofint
%type <cval> charinit
%type <ival> intinit

%start iloc_program



%% /* Beginning of rules */

iloc_program     : instruction_list
                 {
		     first_instruction = $1;
		 }
                 | data_defs instruction_list
                 {
		     first_instruction = $2;
		 }
                 ;

instruction_list : instruction
                 {
		     $$ = $1;
		     EndOfList = $1;
		 }
                 | label_def instruction
                 {
		     Label* label_definition = get_label($1);
		     label_definition->target = $2;
		     $$ = $2;
		     EndOfList = $2;
		 }
                 | instruction_list instruction
                 {
		     EndOfList->next = $2;
		     EndOfList = $2;
		     $$ = $1;
		 }
                 | instruction_list label_def instruction
                 {
		     Label* label_definition = get_label($2);
		     label_definition->target = $3;
		     EndOfList->next = $3;
		     EndOfList = $3;
		     $$ = $1;
		 }
                 ;

instruction      : operation
                 {
		     $$ = malloc(sizeof(Instruction));
		     $$->operations = $1;
		     $$->next = NULL;
		 }
                 | OPEN_BRACKET operation_list CLOSE_BRACKET
                 {
		     $$ = malloc(sizeof(Instruction));
		     $$->operations = $2;
		     $$->next = NULL;
		 }
                 ;

operation_list   : operation
                 {
		     $$ = $1;
		 }
                 | operation SEMICOLON operation_list
                 {
		     $1->next = $3;
		     $$ = $1;
		 }
                 ;

operation        : the_opcode operand_list ARROW operand_list
                 {
		     verify_args($1,$2->num_regs,$2->num_consts+$4->num_consts,
				 $2->num_labels+$4->num_labels,$4->num_regs);
		     $$ = malloc(sizeof(Operation));
		     $$->opcode = $1->name;
		     $$->srcs = $2->regs;
		     $$->consts = append_operands($2->consts,$4->consts);
		     $$->labels = append_operands($2->labels,$4->labels);
		     $$->defs = $4->regs;
		     $$->next = NULL;
		     free($2);
		     free($4);
		 }
                 | the_opcode operand_list
                 {
		     verify_args($1,$2->num_regs,$2->num_consts,$2->num_labels,0);
		     $$ = malloc(sizeof(Operation));
		     $$->opcode = $1->name;
		     $$->srcs = $2->regs;
		     $$->consts = $2->consts;
		     $$->labels = $2->labels;
		     $$->defs = NULL;
		     $$->next = NULL;
		     free($2);
		 }
                 | the_opcode ARROW operand_list
                 {
		     verify_args($1,0,$3->num_consts,$3->num_labels,$3->num_regs);
		     $$ = malloc(sizeof(Operation));
		     $$->opcode = $1->name;
		     $$->srcs = NULL;
		     $$->consts = $3->consts;
		     $$->labels = $3->labels;
		     $$->defs = $3->regs;
		     $$->next = NULL;
		     free($3);
		 }
                 | the_opcode
                 {
		     verify_args($1,0,0,0,0);
		     $$ = malloc(sizeof(Operation));
		     $$->opcode = $1->name;
		     $$->srcs = NULL;
		     $$->consts = NULL;
		     $$->labels = NULL;
		     $$->defs = NULL;
		     $$->next = NULL;
		 }
                 ;

the_opcode       : OPCODE
                 {
		     $$ = current_opcode;
		 }
                 ;

operand_list     : reg
                 {
		     $$ = new_operands();
		     $$->num_regs = 1;
		     $$->regs = $1;
		 }
                 | reg COMMA operand_list
                 {
		     $$ = $3;
		     $$->num_regs += 1;
		     $1->next = $$->regs;
		     $$->regs = $1;
		 }
                 | const
                 {
		     $$ = new_operands();
		     $$->num_consts = 1;
		     $$->consts = $1;
		 }
                 | const COMMA operand_list
                 {
		     $$ = $3;
		     $$->num_consts += 1;
		     $1->next = $$->consts;
		     $$->consts = $1;
		 }
                 | lbl
                 {
		     $$ = new_operands();
		     $$->num_labels = 1;
		     $$->labels = $1;
		 }
                 | lbl COMMA operand_list
                 {
		     $$ = $3;
		     $$->num_labels += 1;
		     $1->next = $$->labels;
		     $$->labels = $1;
		 }
                 ;

reg              : REGISTER
                 {
		     $$ = malloc(sizeof(Operand));
		     //$$->value = (int) strtol(yylval.ival, (char**) NULL, 10);
		     $$->value = yylval.ival;
		     //printf(" visited reg %d %s\n",(yylval.ival),&yytext[1]);
		     $$->next = NULL;
		 }
                 ;

const            : NUMBER
                 {
		     $$ = malloc(sizeof(Operand));
		     $$->value = yylval.ival;
		     //printf(" \n Const: %d \n", yylval.ival);
		     $$->next = NULL;
		 }
		 ;

lbl              : LABEL
                 {
		     $$ = malloc(sizeof(Operand));
		     $$->value = insert_label(token);
		     $$->next = NULL;
		 }
                 ;

label_def        : TARGET
                 {
		   int last_char = strlen(token) - 1; /* take off ':' */
		   token[last_char] = '\0';
		   $$ = insert_label(token);
		 }
                 ;


data_defs      : data_defs data_def
               | data_def
               ;

data_def       : DATA_CHAR addr listofchar { $$ = 0; }
               | DATA_INT  addr listofint  { $$ = 0; }
               ;

listofchar     : listofchar charinit
               | charinit  { $$ = 0; }
               ;

listofint      : listofint intinit
               | intinit
               ;

charinit  :  LABEL { $$ = (unsigned int) token[0]; 
		     set_memory(Start,(char) $$);
		     Start += 1; /* sizeof character in simulator */
                   }
          ;

intinit   :  NUMBER { $$ = atoi(token); 
                      set_word(Start,(int) $$);
		      Start += 4; /* sizeof int in simulator */
                    }
          ;

addr      :  NUMBER 
             { $$ = atoi(token);
               Start = $$;
	     }
          ;          

%% /* Support Code */

/* Create a new initialized Operands structure */
Operands* new_operands()
{
    Operands* operands_ptr = malloc(sizeof(Operands));
    operands_ptr->num_regs = 0;
    operands_ptr->regs = NULL;
    operands_ptr->num_consts = 0;
    operands_ptr->consts = NULL;
    operands_ptr->num_labels = 0;
    operands_ptr->labels = NULL;
    
    return(operands_ptr);
}

/* Append the second list of operands to the end of the first */
Operand* append_operands(Operand* list1, Operand* list2)
{
    Operand* start = list1;

    if (!list1)
	return list2;
    
    while(list1->next)
	list1 = list1->next;

    list1->next = list2;

    return(start);
}

/* debugging code -- kdc */
void PrintOperands( Operand *o ) {
  if (!o)
    fprintf(stdout,"<nil>");
  else while( o ) {
      fprintf(stdout,"%d ",o->value);
      o = o->next;
    }
}
void PrintOperation( Operation *op ) {

  fprintf(stdout,"%s:\tsrcs:\t",opcode_specs[op->opcode].string);
  PrintOperands(op->srcs);
  fprintf(stdout,"\n\tconsts:\t");
  PrintOperands(op->consts);
  fprintf(stdout,"\n\tlabels:\t");
  PrintOperands(op->labels);
  fprintf(stdout,"\n\tdefs:\t");
  PrintOperands(op->defs);
  fprintf(stdout,"\n\n");
}

/* Make sure that the operation was called with the correct number and type
   of arguments */
int verify_args(Opcode* operation,int srcs, int consts, int labels, int defs)
{
    char* error_message;

    if (operation->srcs != srcs)
    {
	error_message = malloc(MAX_ERROR_MESSAGE_LENGTH*sizeof(char));
	sprintf(error_message,"%s used with incorrect number of source registers",
		operation->string);
	yyerror(error_message);
	free(error_message);
	return 0;
    }
    
    if (operation->consts != consts)
    {
	error_message = malloc(MAX_ERROR_MESSAGE_LENGTH*sizeof(char));
	sprintf(error_message,"%s used with incorrect number of constants",
		operation->string);
	yyerror(error_message);
	free(error_message);
	return 0;
    }

    if (operation->labels != labels)
    {
	error_message = malloc(MAX_ERROR_MESSAGE_LENGTH*sizeof(char));
	sprintf(error_message,"%s used with incorrect number of labels (%d vs %d)",
		operation->string,operation->labels,labels);
	yyerror(error_message);
	free(error_message);
	return 0;
    }

    if (operation->defs != defs)
    {
	error_message = malloc(MAX_ERROR_MESSAGE_LENGTH*sizeof(char));
	sprintf(error_message,"%s used with incorrect number of defined registers",
		operation->string);
	yyerror(error_message);
	free(error_message);
	return 0;
    }

    return 1;
}
    
	
void yyerror(char* s)
{
  (void) fprintf(stderr, "Line %d: %s\n", line_counter, s);
  error_found = 1;
}

