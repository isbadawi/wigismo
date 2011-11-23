#!/bin/bash

TESTDIR=../tests

fail=0
echo "*** Testing pretty printer. No output is expected. ***"
for testcase in $TESTDIR/*.wig; do
    if [ -f $TESTDIR/$testcase ]; then
        wigismo -p -s -n -c $TESTDIR/$testcase > test1.wig
        wigismo -p -s -n -c test1.wig > test2.wig
        if ! cmp --silent test1.wig test2.wig; then
            echo "pretty(parse(x)) != pretty(parse(pretty(parse(x))))"
            echo "for x = $TESTDIR/$testcase"
            fail=1 
        fi
        rm -f test1.wig test2.wig
    fi
done
if [ $fail -eq 0 ]; then
    echo "*** Pretty printer tests passed. ***"
fi
