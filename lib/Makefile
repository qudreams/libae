#
#compile the simple event library 'ae'
#########################################

CC=clang
CFLAGS=-I. 
CLFAGS+=-g

OBJS=ae.o anet.o net.o zmalloc.o asBuffer.o

.PHONY:all
all:libae.a
	
libae.a:$(OBJS)
	ar -rv libae.a $(OBJS)

ae.o:ae.c ae.h
	$(CC) $(CFLAGS) -c ae.c
anet.o:anet.c anet.h
	$(CC) $(CFLAGS) -c anet.c
net.o:net.c net.h
	$(CC) $(CFLAGS) -c net.c
zmalloc.o:zmalloc.c zmalloc.h
	$(CC) $(CFLAGS) -c zmalloc.c
asBuffer.o:asBuffer.c asBuffer.h zmalloc.c zmalloc.h
	$(CC) $(CFLAGS) -c asBuffer.c

.PHONY:clean
clean:
	rm -rf *.o libae.a
