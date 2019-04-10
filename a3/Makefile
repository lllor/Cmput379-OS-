valtlb379: TLB.c hashtable.o linklist.o TLB.o
	gcc -g -std=c99 -c TLB.c -lm
	gcc -g -std=c99 -o valtlb379 TLB.o hashtable.o linklist.o -lm
	gcc -g -std=c99 -o heapsort heapsort.c
	gcc -g -std=c99 -o mergesort mergesort.c
	gcc -g -std=c99 -o quicksort quicksort.c

hashtable.o: hashtable.c hashtable.h
	gcc -g -std=c99 -c hashtable.c -lm

linklist.o: linklist.c linklist.h
	gcc -g -std=c99 -c linklist.c -lm

clean:
	rm -f *.o
	rm valtlb379
	rm heapsort
	rm quicksort
	rm mergesort
