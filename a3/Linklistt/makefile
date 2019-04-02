linklist: linklist.o linklist_client.o
	gcc -std=c99 -Wall -g -o linklist linklist.o linklist_client.o

linklist_client.o:	linklist_client.c linklist.h
	gcc -std=c99 -Wall -g -c linklist_client.c

linklist.o:	linklist.c linklist.h
	gcc -std=c99 -Wall -g -c linklist.c

clean:
	rm -f *.o
	rm -f linklist