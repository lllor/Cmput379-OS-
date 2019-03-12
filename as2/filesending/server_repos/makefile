com:	squeue.c squeue.h server.c server.c
	gcc -g -std=c99 -c squeue.c
	gcc -g -std=c99 -pthread `xml2-config --cflags` server.c `xml2-config --libs` -c -lcrypto -lssl -lm
	gcc -std=c99 -pthread `xml2-config --cflags` server.o squeue.o `xml2-config --libs` -o server -lcrypto -lssl -lm
	rm -f *.o
queue:	squeue.h squeue.c
	gcc -g -c squeue.c
	gcc -g -c squeue_test.c
	gcc -g -o squeue_test squeue.o squeue_test.o
clean:
	rm -f server
	rm -f *.o
