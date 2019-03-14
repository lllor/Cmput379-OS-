com:	squeue.c squeue.h server.c server.c client.c
	gcc -g -std=c99 client.c -o ddupclient
	gcc -g -std=c99 -c squeue.c
	gcc -g -std=c99 -pthread `xml2-config --cflags` server.c `xml2-config --libs` -c -lcrypto -lssl -lm
	gcc -std=c99 -pthread `xml2-config --cflags` server.o squeue.o `xml2-config --libs` -o ddupserver -lcrypto -lssl -lm
	rm -f *.o
clean:
	rm -f ddupclient
	rm -f ddupserver
	rm -f *.o
