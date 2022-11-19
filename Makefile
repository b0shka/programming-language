CC=gcc

all: lang

lang: lexer.o parser.o main.o config.o
	$(CC) lexer.c parser.c main.c config/config.c -o lang

main.o: main.c

config.o: config/config.c

lexer.o: main.l
	flex -o lexer.c main.l

parser.o: main.y
	bison -d -o parser.c main.y
