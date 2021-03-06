OBJ = dcc
override CFLAGS += -O3 -flto -Wall -DYYERROR_VERBOSE -std=c++11 `llvm-config --cppflags --libs all --ldflags --system-libs`
PREFIX ?= /usr/local
BINDIR = $(PREFIX)/bin
CXX ?= g++
LEX = flex 
YACC = bison -d --report=all --warnings=all

$(OBJ) : $(OBJ).tab.o lex.yy.o main.o AST.h CodeGenVisitor.h SymbolTable.h  Visitor.h
	$(CXX) -o $@ $(OBJ).tab.o lex.yy.o main.o $(CFLAGS) 

%.o : %.c 
	$(CXX) $^ -c $(CFLAGS)

%.o : %.cpp
	$(CXX) $^ -c $(CFLAGS)

${OBJ}.tab.c : $(OBJ).y
	$(YACC) $(OBJ).y

lex.yy.c : $(OBJ).l $(OBJ).tab.c
	$(LEX) $(OBJ).l

clean :
	rm -f $(OBJ) lex.yy.c ${OBJ}.tab.c ${OBJ}.tab.h ${OBJ}.tab.o lex.yy.o main.o dcc.output

all : ${OBJ}

debug : clean
	$(YACC) --debug  $(OBJ).y
	$(LEX) $(OBJ).l
	$(CXX) $(OBJ).tab.c lex.yy.c main.cpp -g -O0 -DDEBUG -std=c++11 -o $(OBJ) `llvm-config --cppflags --libs all --ldflags --system-libs`

