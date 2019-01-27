convertsigs: squeue.o convertsigs.o
	gcc -g -o convertsigs squeue.o convertsigs.o -lm

convertsigs.o: 	squeue.h convertsigs.c
	gcc -g -c convertsigs.c
squeue.o:       squeue.c squeue.h
	gcc -g -c squeue.c
squeue_test.o:	squeue.h squeue_test.c
	gcc -g -c squeue_test.c
squeue_test:	squeue.o sququq_test.o
	gcc -g -o squeue_test squeue.o squeue_test.o

clean:
	rm -f *.o
	rm -f squeue
	rm -f convertsigs
