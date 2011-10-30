#!/bin/bash

TESTDIR=../tests/type

fail=0
echo "*** Testing type checking phase. Error messages are expected. ***"
for testcase in `ls $TESTDIR`; do
    if wigismo $TESTDIR/$testcase; then
        fail=1
    fi
done

if [ $fail -eq 0 ]; then
    echo "*** Type checking tests passed. ***"
fi
