#!/bin/bash
make clean && make
mkdir -p tests
for i in `find \`pwd\` -name *.dcf | sort`
do
    echo
    echo $i
    ./dcc "$i" > tests/`basename $i .dcf`.ll 2>/dev/null
    if [[ $? == 0 ]]
    then
        clang tests/`basename $i .dcf`.ll -o tests/`basename $i .dcf`.out 2>/dev/null
        ./tests/`basename $i .dcf`.out
    else
        echo "Compiler Error"
    fi
    echo
done

