#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

FILE *yyin;
int yylineno; 
int errorCount = 0;
//int yyparse();

/* 
// test scanner
int main (int argc, char* argv[]) {
	if (argc == 2) {
		FILE *fp;
		fp = fopen(argv[1], "r");
		yyin = fp;
	} else {
		yyin = stdin;
	}
	yylex();
	return 0;
}
*/

int yywrap(void) {
    return 1;
}

int yyerror (const char *s) {	
	errorCount += 1;
	fprintf(stderr, "Line %d: %s\n", yylineno, s);
}

int main (int argc, char* argv[]) {
	if (argc == 2) {
		if (!strcmp("-h", argv[1])) {
			printf("-h : help\n");
			return 0;
		} else {
			if((access(argv[1], F_OK)) != -1) {   
        		FILE *fp;
				fp = fopen(argv[1], "r");
				yyin = fp;
    		} else {
    			printf("Cannot find file %s.\n", argv[1]);
    			return 0;
   			}		
		}
	} else {
		yyin = stdin;
	}

	yyparse();

	if (errorCount == 0) {
		printf("Parse Successfully.\n");
	} else {
		printf("Parser fails. Total number of errors: %d.\n", errorCount);
	}
	return 0;
}
