#!/bin/bash

source "tf.sh"

for i in `ls $CASEDIR/*.log`
do
	base="${i%.*}"
	if [ -f $base.in ]
	then
		INPUT="<$base.in"
	else
		INPUT=''
	fi
	testfunc $base "./$PROG -v $base.obj $INPUT" "$base.out" "$base.log" "diff -bwi"
done

echo -e '\E[;32m'"\033[1m $TESTPASS OUT OF $TESTCASE TEST(S) PASSED\033[0m" 

# vim: tw=0
