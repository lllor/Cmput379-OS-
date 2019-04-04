test: test.o hashtable.o
	gcc -std=c99 -Wall -g -o test hashtable.o test.o

test.o:	test.c
	gcc -std=c99 -Wall -g -c test.c

hashtable.o:	hashtable.c hashtable.h
	gcc -std=c99 -Wall -g -c hashtable.c

clean:
	rm -f *.o
	rm -f test