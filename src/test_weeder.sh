#!/bin/bash

WEEDDIR=../tests/weed

for testcase in `ls ../tests/weed`; do
    wigismo $WEEDDIR/$testcase
done;
