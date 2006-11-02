# Makefile for building LPC on Mac and Linux.
# Requires GNU Bison 2.4 and GNU Flex 2.5.35

VERSION = 3.64
CC = g++
OUT = lpc
INSTALL_DIR = /opt/lpc/

OS = $(uname -a | '{awk print $1}')
ifeq ($(OS),linux)
	CFLAGS = -Wall -Wno-sign-compare -static
	LDFLAGS = 
else
	CFLAGS = -Wall -Wno-sign-compare
	LDFLAGS = 
endif

OBJECTS = stacknode.o symbolstack.o functiontable.o tipos.o ast.o samp.tab.o lex.yy.o

all: $(OUT)

install: $(OUT)
	rm -rdf $(INSTALL_DIR)
	mkdir $(INSTALL_DIR)
	cp -r ../bin $(INSTALL_DIR)/
	cp -r $(OUT) $(INSTALL_DIR)/bin/
	mkdir $(INSTALL_DIR)/lib
	mv $(INSTALL_DIR)/bin/lpstdlib.j $(INSTALL_DIR)/lib/lpstdlib.j
	@echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
	@echo "LPC installed successfully."
	@echo "Please export $(INSTALL_DIR) as LPC_HOME in your environment then"
	@echo "invoke LPC_HOME/bin/compiler.sh <file> to build your source files."
	@echo "...and remember to have a lot of fun!"

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

samp.tab.cpp: samp.y
	bison -d samp.y -o samp.tab.cpp

lex.yy.cpp: samp.l
	flex -o lex.yy.cpp samp.l


$(OUT): $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJECTS) -o $(OUT)


zip:
	zip L+_v$(VERSION).zip samp.y samp.l ast.h ast.cpp tipos.h CNodoPila.cpp CPilaDeSimbolos.cpp CNodoPila.h CPilaDeSimbolos.h Makefile *.lp CFunctionTable.h CFunctionTable.cpp tipos.cpp lpstdlib.j

clean:
	rm -f $(OBJECTS)
	rm -f $(OUT)
	rm -f samp.tab.cpp
	rm -f samp.tab.hpp
	rm -f lex.yy.cpp
	
