
CC = gcc
CPP = g++

CFLAGS = -O
LFLAGS = -lg++ /usr/local/lib/libg++.a

OBJS=	util.o \
	cgimain.o

.cc.o:
	$(CC) -c $(CFLAGS) $<

all: test imagemap adder status status2 status3 pgif

imagemap: util.o imagemap.o
	$(CC) util.o imagemap.o -o imagemap

test: test.o $(OBJS)
	$(CC) test.o $(OBJS) -o test

adder: adder.o $(OBJS)
	$(CC) adder.o $(OBJS) -o adder

status: status.o $(OBJS)
	$(CC) status.o $(OBJS) -o status

status2: status2.o $(OBJS)
	$(CC) status2.o $(OBJS) -o status2

status3: status3.o $(OBJS)
	$(CC) status3.o $(OBJS) -o status3

pgif: pgif.o
	$(CC) pgif.o -o pgif

pgif2: pgif2.o
	$(CC) pgif2.o -o pgif2

clean:
	rm -f *.o

