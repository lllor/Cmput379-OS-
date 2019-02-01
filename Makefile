#convertsigs: squeue.o convertsigs.o
#	gcc -g -std=c99 -o convertsigs squeue.o convertsigs.o -lm

#convertsigsDouble.o: 	squeue.h convertsigs.c
#	gcc -g -std=c99 -c convertsigs.c
#conversigsSingle.o:	squeue.h convertsigs.c
#	gcc -g -std=c99 -c -DSINGLE convertsigs.c
squeue.o:       squeue.c squeue.h
	gcc -g -std=c99 -c squeue.c
convertsigs.single:	convertsigs.c squeue.o convertsigs.o
	gcc -g -std=c99 -c -DSINGLE convertsigs.c
	gcc -g -std=c99 -o convertsigs squeue.o convertsigs.o -lm
convertsigs.double:	convertsigs.c squeue.o convertsigs.o
	gcc -g -std=c99 -c convertsigs.c
	gcc -g -std=c99 -o convertsigs squeue.o convertsigs.o -lm

clean:
	rm -f *.o
	rm -f squeue
	rm -f convertsigs
CC = gcc
CFLAGS = -Wall -pedantic -Werror
