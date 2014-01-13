TITLE=p3test
FILES=cases Makefile test.sh

facts: facts.c
	gcc -o facts facts.c

package: ${FILES}
	rm -rf ~/tmp/${TITLE}
	mkdir ~/tmp/${TITLE} || true
	cp -a ${FILES} ~/tmp/${TITLE}
	tar -C ~/tmp -czf ${TITLE}.tar.gz ${TITLE} 

testpack:
	rm -rf ~/tmp/${TITLE}/
	tar -xvf ${TITLE}.tar.gz -C ~/tmp/
	gcc -o ~/tmp/${TITLE}/facts facts.c

# vim: tw=0
