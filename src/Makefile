CC = gcc

BISON = bison -y -d

CFLAGS = -Wall -pedantic -g

all:
	make code
	make -s check

code: y.tab.o lex.yy.o main.o tree.h tree.o memory.h memory.o error.h error.o pretty.o pretty.h weed.o symbol.o weed.h
	$(CC) lex.yy.o y.tab.o tree.o memory.o error.o pretty.o weed.o symbol.o chash/chash.o main.o -o wigismo -lfl

y.tab.c y.tab.h: wig.y
	$(BISON) wig.y

lex.yy.c: wig.l y.tab.h
	flex wig.l

check: 
	make -s pretty
	make -s weed
	make -s unit

weed: 
	./test_weeder.sh

pretty:
	./test_printer.sh

unit:
	@gcc unit_tests.c tree.h tree.o memory.h memory.o weed.o weed.h -o tests
	@./tests
	@rm tests

clean:
	rm -f *.o lex.* y.tab.*
