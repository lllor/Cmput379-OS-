convertsigs: squeue.o convertsigs.o
	gcc -g -o convertsigs squeue.o convertsigs.o -lm

convertsigs.o: 	squeue.h convertsigs.c
	gcc -g -c -DSINGLE convertsigs.c
squeue.o:       squeue.c squeue.h
	gcc -g -c squeue.c
clean:
	rm -f *.o
	rm -f squeue
	rm -f convertsigs
