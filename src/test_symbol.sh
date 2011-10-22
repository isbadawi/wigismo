#!/bin/bash

TESTDIR=../tests/symbol

fail=0
echo "*** Testing symbol table phase. Error messages are expected. ***"
for testcase in `ls $TESTDIR`; do
    if wigismo $TESTDIR/$testcase; then
        fail=1
        echo $testcase
    fi
done

if [ $fail -eq 0 ]; then
    echo "*** Symbol table tests passed. ***"
fi
