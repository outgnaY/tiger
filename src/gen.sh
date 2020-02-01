#!/bin/bash

BIN=gen
OBJDIR=./lex
TESTCASEDIR=../testcases
rm -f $OBJDIR/*
for tcase in `ls $TESTCASEDIR`
do
    ./$BIN $TESTCASEDIR/$tcase >& $OBJDIR/$tcase.out
done