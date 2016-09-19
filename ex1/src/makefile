CC=gcc

CFLAGS = -g -c -Wall -pedantic
#CFLAGS = -ansi -c -Wall -pedantic
objects = ncp.o proto.o sendto_dbg.o

all: ncp rcv 

proto.o: proto.c proto.h
	    $(CC) -c proto.c

ncp: ncp.c proto.o sendto_dbg.o
	    $(CC) ncp.c proto.o sendto_dbg.o -o ncp

sendto_dbg.o: sendto_dbg.c
	    $(CC) -c sendto_dbg.c

rcv: rcv.c proto.o sendto_dbg.o
	    $(CC) rcv.c proto.o sendto_dbg.o -o rcv 

clean:
	rm *.o
	rm ncp 
	rm sendto_dbg
	rm proto 
	rm rcv

%.o:    %.c
	$(CC) $(CFLAGS) $*.c


