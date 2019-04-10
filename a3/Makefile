
valtlb379: TLB.c hashtable.o linklist.o TLB.o
	gcc -g -c TLB.c -lm
	gcc -g -o valtlb379 TLB.o hashtable.o linklist.o -lm
	gcc -g -o heapsort heapsort.c
	gcc -g -o mergesort mergesort.c
	gcc -g -o quicksort quicksort.c

hashtable.o: hashtable.c hashtable.h
	gcc -g -c hashtable.c -lm

linklist.o: linklist.c linklist.h
	gcc -g -c linklist.c -lm

clean:
	rm -f *.o
	rm valtlb379
	rm heapsort
	rm quicksort
	rm mergesort