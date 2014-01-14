#!/bin/bash

source "tf.sh"

testfunc cases/prime "./$PROG -v cases/prime.obj" "cases/prime.out" "cases/prime.sha" "sha1sum"

echo -en '\E[;32m'"\033[1m $TESTPASS OUT OF $TESTCASE TEST(S) PASSED\033[0m" 

# vim: tw=0
