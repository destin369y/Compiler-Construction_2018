# Makefile

CC = gcc

all: demo

parser.tab.c parser.tab.h: parser.y
	bison -d parser.y

lex.yy.c: scanner.lex parser.tab.h
	flex scanner.lex

demo: parser.tab.c parser.tab.h lex.yy.c main.c
	$(CC) -o demo parser.tab.c lex.yy.c main.c 

clean:
	rm demo parser.tab.c parser.tab.h lex.yy.c
	