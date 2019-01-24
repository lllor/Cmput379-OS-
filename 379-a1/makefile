queue: squeue.o squeue_test.o
	gcc -std=c99 -Wall -g -o squeue squeue.o squeue_test.o

squeue_test.o:        squeue_test.c squeue.h
	gcc -std=c99 -Wall -g -c squeue_test.c

squeue.o:       squeue.c squeue.h
	gcc -std=c99 -Wall -g -c squeue.c
clean:
	rm -f *.o
	rm -f squeue
