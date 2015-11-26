#!/bin/bash
for i in `find \`pwd\` -name *.dcf`
do
    echo
    echo $i
    ./dcc "$i" > tests/`basename $i`.ll
    clang tests/`basename $i`.ll
    echo
done

