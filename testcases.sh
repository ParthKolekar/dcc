#!/bin/bash
make clean && make
for i in `find \`pwd\` -name *.dcf`
do
    echo
    echo $i
    ./dcc "$i" > tests/`basename $i`.ll
    clang tests/`basename $i`.ll -o tests/`basename $i`.out
    ./tests/`basename $i`.out
    echo
done

