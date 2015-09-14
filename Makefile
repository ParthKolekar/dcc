OBJ = dcc
override CFLAGS += -O3 -flto -funroll-loops -Wall 
PREFIX ?= /usr/local
BINDIR = $(PREFIX)/bin
CXX ?= g++
LEX = flex 
YACC = bison -d --debug

$(OBJ) : ${OBJ}.tab.o lex.yy.o main.o
	$(CXX) $^ -o $@ $(CFLAGS)

%.o : %.c 
	$(CXX) $^ -c $(CFLAGS)

%.o : %.cpp
	$(CXX) $^ -c $(CFLAGS)

${OBJ}.tab.c : ${OBJ}.y
	$(YACC) ${OBJ}.y

lex.yy.c : ${OBJ}.l ${OBJ}.tab.c
	$(LEX) ${OBJ}.l

clean :
	rm -f $(OBJ) lex.yy.c ${OBJ}.tab.c ${OBJ}.tab.h ${OBJ}.tab.o lex.yy.o main.o

all : ${OBJ}
