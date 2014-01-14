#!/bin/bash

CURRENT=`pwd`
CASEDIR=cases
SOURCE="$1"
TITLE=Program4
PROG=pdp8
TMPDIR=tmpdir
TESTCASE=0
TESTPASS=0
TESTFAIL=0
TESTLEAK=0

function testcore {
	cmdline=$1
	referenceout=$2
	referenceverbose=$3
	difftool=$4

	out=$TMPDIR/$TESTCASE.out
	log=$TMPDIR/$TESTCASE.log
	difflog=$TMPDIR/$TESTCASE.diff

	if [ "$difftool" != "sha1sum" ]
	then
		#echo "timeout -s 9 15s $cmdline >$out 2>$log"
		bash -c "timeout -s 9 15s $cmdline >$out 2>$log"
		sed "s/(NOP)/()/" < $log | sed -r "s/\s+/ /g" | tr "[:lower:]" "[:upper:]" > $TMPDIR/tmp
		mv $TMPDIR/tmp $log
	else
		#bash -c "timeout -s 9 60s $cmdline > $out 2> >(sed 's/(NOP)/()/' | sed -r 's/\s+/ /g' | tr \"[:lower:]\" \"[:upper:]\" | sha1sum >$log)"
		bash -c "timeout -s 9 60s $cmdline > $out 2> >(sed 's/(NOP)/()/' | tr -d \"[:space:]\" | tr \"[:lower:]\" \"[:upper:]\" | sha1sum >$log)"
		#bash -c "time $cmdline > $out 2> >(sed 's/(NOP)/()/' | sed -r 's/\s+/ /g' | tr \"[:lower:]\" \"[:upper:]\" | sha1sum >$log)"
		#bash -c "time $cmdline > $out 2> >(sed 's/(NOP)/()/' | tr -d \"[:space:]\" | tr \"[:lower:]\" \"[:upper:]\" | sha1sum >$log)"
	fi
	RET=$?

	if [ $RET == 139 ]
	then
		SEGF=1
		FAILED=1
	fi
	
	if [ $RET == 137 ]
	then
		SEGF=1
		FAILED=1
		TLE=1
	fi

	#bash -c "valgrind --log-file=$vglog $cmdline | tr -d ' ' > $out "
	#bash -c "$cmdline > $out "
	#echo $RET

	diff -bwi $out $referenceout > $difflog
	RET=$?

	if [ $RET != 0 ]
	then 
		ANSFAILED=1
		VERBFAILED=1
		echo "See $out for your output and $difflog for diagnose"
	fi

	$difftool $log $referenceverbose > $difflog
	RET=$?

	if [ $RET != 0 ]
	then
		VERBFAILED=1
	fi
}

function testfunc {
	ANSFAILED=0
	VERBFAILED=0
	SEGF=0
	TLE=0
	echo -n "Testing $1""..."
	testcore "${@:2}"
	let "TESTCASE=TESTCASE+1"
	let "WA=WA+ANSFAILED"
	let "WV=WV+VERBFAILED"
	if [ $VERBFAILED -eq 0 ]
	then
		echo -e '...\E[;32m'"\033[1mPASS\033[0m"
		let "TESTPASS=TESTPASS+1"
	elif [ $ANSFAILED -eq 0 ]
	then
		echo -e '...\E[1;33m'"\033[1mVERBOSE\033[0m"
	elif [ $TLE -ne 0 ]
	then
		echo -e '...\E[;31m'"\033[1m\"$1\"TIME LIMIT EXCEEDED\033[0m"
	elif [ $SEGF -ne 0 ]
	then
		echo -e '...\E[;31m'"\033[1m\"$1\"SEGFAULT\033[0m"
	else
		echo -e '...\E[;31m'"\033[1m\"$1\"WRONG OUTPUT\033[0m"
	fi
}

if [ ! -f $PROG ]
then
	cd $SOURCE
	make
	cp pdp8 $CURRENT
	cd $CURRENT
fi

if [ ! -f $PROG ]
then
	echo "[$TITLE] Could not build $PROG at $SOURCE. Please check your source code or Makefile";
	exit
fi

if [ ! -d $TMPDIR ]
then
	mkdir $TMPDIR 
fi

rm -f $TMPDIR/*

# vim: tw=0
