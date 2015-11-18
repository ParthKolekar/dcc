#include <iostream>
#include <cstdio>
#include "AST.h"
#include "Visitor.h"
#include "PrintVisitor.cpp"

extern "C" FILE *yyin; 
extern "C" int yyparse(); 

ASTProgram * start;

llvm::Module *TheModule;
llvm::IRBuilder<> Builder(llvm::getGlobalContext());
std::map<std::string, llvm::Value*> NamedValues;

llvm::Value * ErrorV(const char * error) {
    std::cerr << error;
    return 0;
}

int main (const int argc, const char ** argv) {
    if (argc < 2) {
        yyin = NULL;
        std::cerr <<"fatal error: no input files" << std::endl;
        return -1;
    } else {
        FILE * infile = fopen(argv[1], "r");
        if (!infile) {
            std::cerr <<"Error reading file " << argv[1] << std::endl;
            return -1;
        }
        yyin = infile;
    }
#ifdef DEBUG
    extern int yydebug;
    yydebug = 1;
#endif
    if (!yyparse()) {
        llvm::LLVMContext &Context = llvm::getGlobalContext();
        TheModule = new llvm::Module("my cool jit", Context);
        TheModule->dump();
        // PrintVisitor * visitor = new PrintVisitor();
        // visitor->visit(start);
    }
    return 0;
}

