###################################################
#
# Makefile
# Simple TCP/IP communication using sockets
#
# Vangelis Koukis <vkoukis@cslab.ece.ntua.gr>
#
###################################################

CC = gcc

CFLAGS = -Wall
CFLAGS += -g
# CFLAGS += -O2 -fomit-frame-pointer -finline-functions

LIBS =

BINS = socket-server socket-client socket-server1

all: $(BINS)

socket-server: socket-server.c socket-common.h crypto-test.h
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

socket-server1: socket-server.c socket-common1.h
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)


socket-client: socket-client.c socket-common.h crypto-test.h
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

clean:
	rm -f *.o *~ $(BINS)
