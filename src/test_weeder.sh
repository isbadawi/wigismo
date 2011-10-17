#!/bin/bash

WEEDDIR=../tests/weed

fail=0
echo "*** Testing weeder. Error messages are expected. ***"
for testcase in `ls ../tests/weed`; do
    if wigismo $WEEDDIR/$testcase; then
        fail=1
    fi
done

if [ $fail -eq 0 ]; then
    echo "*** Weeder tests passed. ***"
fi
