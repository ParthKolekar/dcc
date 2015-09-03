#include <iostream>
#include <cstdio>

extern "C" FILE *yyin; 
extern "C" int yyparse(); 

int main (const int argc, const char ** argv) {
    if (argc < 2) {
        yyin = stdin;
    } else {
        FILE *infile = fopen(argv[1], "r");
        if (!infile) {
            std::cerr <<"Error reading file " << argv[1] << std::endl;
            return -1;
        }
        yyin = infile;
    }
    do {
        yyparse();
    } while (!feof(yyin));
    return 0;
}

