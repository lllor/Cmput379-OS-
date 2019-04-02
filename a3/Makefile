valtlb379: TLB.c linklist.c linklist.h
	gcc -g -std=c99 -c linklist.c
	gcc -g -std=c99 -c TLB.c
	gcc -g -std=c99 TLB.o linklist.o -o valtlb379 
	gcc -g -std=c99 -o heapsort heapsort.c
	gcc -g -std=c99 -o mergesort mergesort.c
	gcc -g -std=c99 -o quicksort quicksort.c
	rm -f *.o

clean:
	rm -f *.o
	rm valtlb379
	rm heapsort
	rm quicksort
	rm mergesort