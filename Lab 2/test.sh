#!/bin/bash

function test5b {
	echo -n "Testing $1 "
	./5bit $3 < $1 | ./filter $3 > tmp || true #echo -e '\E[;31m'"\033[1mFAIL\033[0m"
	./filter $3 < $2 > stdtmp
	DIFF=`diff -q tmp stdtmp`
	if [ ! -z "$DIFF" ]
	then
		#echo "[Program2] Failed to translate $1 to $2"
		echo -n "with filename...";
		./5bit $3 $1 | ./filter $3 > tmp || true #echo -e '\E[;31m'"\033[1mFAIL\033[0m"
		./filter $3 < $2 > stdtmp
		DIFF=`diff -q tmp stdtmp`
		if [ ! -z "$DIFF" ]
		then
			echo -e '\E[;31m'"\033[1mFAIL\033[0m"
			echo "Run command ./5bit $3 < $1 | ./filter $3 > tmp"
			echo "After this, check output and file tmp to see what is wrong"
		else
			echo -e '\E[;32m'"\033[1mPASS\033[0m"
		fi
	else
		echo -e '...\E[;32m'"\033[1mPASS\033[0m"
	fi
}

if [ ! -f filter ]
then
	make filter
fi


if [ ! -f 5bit ]
then
	echo "[Program2] Could not find 5bit. Please compile your source code.";
	exit
fi

if [ ! -x 5bit ]
then
	chmod +x 5bit
fi

test5b "four.txt" "four.5b" ""
test5b "gettysburg.txt" "gettysburg.5b" ""
test5b "smile.gif" "smile.5b" ""

test5b "four.5b" "four.txt" "-d"
test5b "gettysburg.5b" "gettysburg.txt" "-d"
test5b "smile.5b" "smile.gif" "-d"

# vim: tw=0
