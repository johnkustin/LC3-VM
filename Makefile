CC=gcc
CFLAGS=-Og
OBJS = lc3.o lc3instructions.o 

all: lc3

lc3: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

lc3instructions.o: lc3instructions.c  lc3instructions.h
	$(CC) $(CFLAGS) -c $<

.PHONY:
clean:
	rm -f lc3 *.o
