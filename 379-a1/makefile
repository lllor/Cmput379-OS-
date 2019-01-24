convertsigs: squeue.o convertsigs.o
	gcc -std=c99 -Wall -g -o convertsigs squeue.o convertsigs.o

convertsigs.o: 	squeue.h convertsigs.c
	gcc -std=c99 -Wall -g -c convertsigs.c
squeue.o:       squeue.c squeue.h
	gcc -std=c99 -Wall -g -c squeue.c
clean:
	rm -f *.o
	rm -f squeue
	rm -f convertsigs
