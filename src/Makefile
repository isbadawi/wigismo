CC = gcc

BISON = bison -y -d

CFLAGS = -Wall -pedantic -g

code: y.tab.o lex.yy.o main.o tree.h tree.o memory.h memory.o error.h error.o pretty.o pretty.h
	$(CC) lex.yy.o y.tab.o tree.o memory.o error.o pretty.o main.o -o wigismo -lfl

y.tab.c y.tab.h: wig.y
	$(BISON) wig.y

lex.yy.c: wig.l y.tab.h
	flex wig.l

check: 
	make weed
	make pretty
	make unit

weed: 
	./test_weeder.sh

pretty:
	./test_printer.sh

clean:
	rm -f *.o lex.* y.tab.*
