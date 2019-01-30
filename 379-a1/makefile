convertsigs: squeue.o convertsigs.o
	gcc -g -std=c99 -o convertsigs squeue.o convertsigs.o -lm

convertsigs.o: 	squeue.h convertsigs.c
	gcc -g -std=c99 -c convertsigs.c
squeue.o:       squeue.c squeue.h
	gcc -g -std=c99 -c squeue.c
clean:
	rm -f *.o
	rm -f squeue
	rm -f convertsigs
