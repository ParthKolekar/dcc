#!/bin/bash
for i in `find \`pwd\` -name *.dcf`
do
    echo
    echo $i
    ./dcc "$i" >/dev/null
    echo
done

