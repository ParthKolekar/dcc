#!/bin/bash
make clean && make
mkdir -p tests
for i in `find \`pwd\` -name *.dcf`
do
    echo
    echo $i
    ./dcc "$i" > tests/`basename $i .dcf`.ll
    if [[ $? == 0 ]]
    then
        clang tests/`basename $i .dcf`.ll -o tests/`basename $i .dcf`.out
        ./tests/`basename $i .dcf`.out
    else
        echo "Compiler Error"
    fi
    echo
done

