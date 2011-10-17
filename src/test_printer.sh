#!/bin/bash

TESTDIR=../tests

for testcase in `ls $TESTDIR`; do
    if [ -f $TESTDIR/$testcase ]; then
        wigismo -p $TESTDIR/$testcase > test1.wig
        wigismo -p test1.wig > test2.wig
        if ! cmp --silent test1.wig test2.wig; then
            echo "pretty(parse(x)) != pretty(parse(pretty(parse(x))))"
            echo "for x = $TESTDIR/$testcase"
        fi
        rm -f test1.wig test2.wig
    fi
done
