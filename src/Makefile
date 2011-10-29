CC = gcc

BISON = bison -y -d

CFLAGS = -Wall -pedantic -g

all:
	make code
	make -s check

code: y.tab.o lex.yy.o main.o tree.h tree.o memory.h memory.o error.h error.o pretty.o pretty.h weed.o weed.h symbol.o symbol.h type.o type.h chash/chash.o
	$(CC) lex.yy.o y.tab.o tree.o memory.o error.o pretty.o weed.o symbol.o type.o chash/chash.o main.o -o wigismo -lfl

chash/chash.o:
	cd chash; make

y.tab.c y.tab.h: wig.y
	$(BISON) wig.y

lex.yy.c: wig.l y.tab.h
	flex wig.l

check: 
	make -s pretty
	make -s weed
	make -s unit
	make -s symbol
	make -s type

weed: 
	./test_weeder.sh

pretty:
	./test_printer.sh

symbol:
	./test_symbol.sh

type:
	./test_type.sh

unit:
	@gcc unit_tests.c tree.h tree.o memory.h memory.o weed.o weed.h -o tests
	@./tests
	@rm tests

clean:
	rm -f *.o lex.* y.tab.*
	rm -f chash/chash.o
