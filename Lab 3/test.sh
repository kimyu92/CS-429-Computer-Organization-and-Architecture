#!/bin/bash

CASEDIR=cases
TITLE=Program3
PROG=facts
TMPDIR=tmpdir
TESTCASE=0
TESTFAIL=0
TESTLEAK=0

function testcore {
	cmdline=$1
	reference=$2
	difftool=$3
	testname=$4

	out=$TMPDIR/$TESTCASE.out
	vglog=$TMPDIR/$TESTCASE.log
	#echo $cmdline
	bash -c "valgrind --log-file=$vglog $cmdline > $out "
	#bash -c "$cmdline > $out "
	RET=$?
	#echo $RET

	if [ "$reference" == "UNSPECIFIED" ]
	then
		FAILED=0
	else
		DIFF=`$difftool $out $reference`
		if [ $? != 0 ]
		then 
			FAILED=1
			echo "Case $1 was not properly answered. Please review $out for the output of your program"
			echo $DIFF | head -4
		fi
	fi
	grep "SIGSEGV" $vglog >/dev/null 2>/dev/null
	if [ $? == 0 ]
	then
		FAILED=1
	else
		grep "All heap blocks were freed -- no leaks are possible" $vglog > /dev/null 2>/dev/null
		LEAK=$?
	fi
	let "TESTCASE=TESTCASE+1"
	if [ $FAILED -ne 0 ]
	then
		let "TESTFAIL=TESTFAIL+1"
	elif [ $LEAK -ne 0 ]
	then
		let "TESTLEAK=TESTLEAK+1"
	fi
}

function testfunc {
	FAILED=0
	LEAK=0
	echo -n "Testing $1"
	"${@:2}"
	if [ $FAILED -eq 0 ]
	then
		if [ $LEAK -ne 0 ]
		then
			echo -e '...\E[1;33m'"\033[1mLEAKS\033[0m"
		else
			echo -e '...\E[;32m'"\033[1mPASS\033[0m"
		fi
	else
		echo -e '...\E[;31m'"\033[1mTest \"$1\" FAILED\033[0m"
	fi
}

if [ ! -f $PROG ]
then
	echo "[$TITLE] Could not find $PROG. Please compile your source code.";
	exit
fi

if [ ! -x $PROG ]
then
	chmod +x $PROG 
fi

if [ ! -d $TMPDIR ]
then
	mkdir $TMPDIR 
fi

rm -f $TMPDIR/*

echo "Compliance Tests..."
testfunc "No Facts File" testcore "./$PROG < /dev/null" "/dev/null" "diff"
testfunc "No Facts" testcore "./$PROG /dev/null cases/nofacts.Q" "cases/nofacts.A" "diff"
testfunc "Invalide Filename" testcore "./$PROG THISFILE SHOULDNOTEXIST 2>/dev/null" "/dev/null" "diff"
testfunc "Invalid Format" testcore "./$PROG cases/BADF cases/computers.q 2>/dev/null" "UNSPECIFIED" "diff"

for i in `ls $CASEDIR/*.a`
do
	base="${i%.*}"
	testfunc $base testcore "./$PROG $base.fax $base.q" "$base.a" "diff"
done

let "TESTPASS=TESTCASE-TESTFAIL-TESTLEAK"
if [ $TESTPASS  -eq $TESTCASE ]
then
	echo -e '\E[;32m'"\033[1mALL $TESTCASE TEST(S) PASSED\033[0m" 
else
	echo -en '\E[;31m'"\033[1m$TESTFAIL TEST(S) FAILED"
	if [ $TESTLEAK -ne 0 ]
	then
		echo -en '\E[1;33m'"\033[1m AND $TESTLEAK TEST(S) LEAKED"
	fi
	echo -en '\E[;32m'"\033[1m OUT OF $TESTCASE TEST(S)\033[0m" 
	echo
fi

exit

# vim: tw=0
