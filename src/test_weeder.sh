#!/bin/bash

TESTDIR=../tests/weed

fail=0
echo "*** Testing weeder. Error messages are expected. ***"
for testcase in `ls $TESTDIR`; do
    if wigismo -c $TESTDIR/$testcase; then
        fail=1
    fi
done

if [ $fail -eq 0 ]; then
    echo "*** Weeder tests passed. ***"
fi
