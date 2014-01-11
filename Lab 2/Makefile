FILES=four.5b four.txt gettysburg.5b gettysburg.txt Makefile smile.5b smile.gif filter.c test.sh

5bit: 5bit.c
	gcc -o 5bit 5bit.c

filter: filter.c
	gcc -o filter filter.c

selftest.tar.gz: ${FILES}
	rm -rf ../selftest
	mkdir ../selftest || true
	cp ${FILES} ../selftest
	tar -C .. -czf selftest.tar.gz selftest/
