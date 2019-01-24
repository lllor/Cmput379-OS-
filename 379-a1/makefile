convertsigs: squeue.o convertsigs.o
	gcc -std=c99 -Wall -g -o convertsigs squeue.o convertsigs.o

convertsigs.o: 	squeue.h convertsigs.c
	gcc -std=c99 -Wall -g -c convertsigs.c
squeue.o:       squeue.c squeue.h
	gcc -std=c99 -Wall -g -c squeue.c
squeue_test.o:	squeue.h squeue_test.c
	gcc -std=c99 -Wall -g -c squeue_test.c
squeue_test:	squeue.o sququq_test.o
	gcc -std=c99 -Wall -g -o squeue_test squeue.o squeue_test.o

clean:
	rm -f *.o
	rm -f squeue
	rm -f convertsigs
