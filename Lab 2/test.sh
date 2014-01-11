#!/bin/bash

TMPDIR=tmpdir
TESTCASE=0
TESTFAIL=0

function testcore {
	cmdline=$1
	reference=$2
	difftool=$3
	testname=$4

	out=$TMPDIR/$TESTCASE.out
	bash -c "$cmdline > $out"

	if [ $? != 0 ]
	then
		FAILED=1;
	else
		DIFF=`$difftool $out $2`
		if [ $? != 0 ]
		then 
			FAILED=1
			echo "File $1 was not properly $4. Please review $out for the output of your program"
			echo $DIFF | head -4
		fi
	fi
	let "TESTCASE=TESTCASE+1"
	if [ $FAILED -ne 0 ]
	then
		let "TESTFAIL=TESTFAIL+1"
	fi
}

function testpair {
	plain=$1
	coded=$2
	echo -n "Testing Group: $plain<->$coded "
	FAILED=0
	testcore "./5bit < $plain" "$coded" "diff" "encoded from stdin"
	testcore "./5bit $plain" "$coded" "diff" "encoded from file"
	testcore "./5bit -d < $coded" "$plain" "cmp -b" "decoded from stdin"
	testcore "./5bit -d $coded" "$plain" "cmp -b" "decoded from file"
	if [ $FAILED -eq 0 ]
	then
		echo -e '...\E[;32m'"\033[1mPASS\033[0m"
	fi
}

function testpriority {
	echo -n "Testing Group: Priority between filename and stdio"
	I=$TMPDIR/sh
	testcore "./5bit < /dev/null smile.gif" "smile.5b" "diff" "encoded, filename should have higher priority than stdin"
	testcore "./5bit -d < /dev/null smile.5b" "smile.gif" "cmp -b" "decoded, filename should have higher priority than stdin"
}

function testchain {
	cp /bin/sh $TMPDIR/
	echo -n "Testing Group: Chaining I/O"
	I=$TMPDIR/sh
	testcore "./5bit < $I | ./5bit | ./5bit | ./5bit | ./5bit -d | ./5bit -d | ./5bit -d | ./5bit -d " "$I" "cmp -b" "working on chaining"
}

function testfunc {
	FAILED=0
	$1
	if [ $FAILED -eq 0 ]
	then
		echo -e '...\E[;32m'"\033[1mPASS\033[0m"
	fi
}

if [ ! -f 5bit ]
then
	echo "[Program2] Could not find 5bit. Please compile your source code.";
	exit
fi

if [ ! -x 5bit ]
then
	chmod +x 5bit
fi

if [ ! -d $TMPDIR ]
then
	mkdir $TMPDIR 
fi

rm -f $TMPDIR/*

testpair "four.txt" "four.5b"
testpair "gettysburg.txt" "gettysburg.5b"
testpair "smile.gif" "smile.5b"
testpair "/dev/null" "/dev/null"
testpair "45zero" "45zero.5b"
testfunc testpriority
testfunc testchain

#let "TESTPASS=TESTCASE-TESTFAIL"
if [ $TESTFAIL -eq 0 ]
then
	echo -e '\E[;32m'"\033[1mALL $TESTCASE TESTS PASSED\033[0m" 
fi

exit

# vim: tw=0
