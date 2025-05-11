# Tauha Imran, 22i-1239
# Hussain Ali, 22i-0902

CC = gcc
CFLAGS = -Wall -g -Wno-unused-function
BISON = bison
FLEX = flex

all: json2relcsv

json2relcsv: lex.yy.o parser.tab.o ast.o csv.o main.o
	$(CC) $(CFLAGS) -o $@ $^ -lfl

lex.yy.c: scanner.l parser.tab.h
	$(FLEX) --header-file=lex.yy.h scanner.l

parser.tab.c parser.tab.h: parser.y
	$(BISON) -d parser.y

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o lex.yy.c lex.yy.h parser.tab.c parser.tab.h json2relcsv