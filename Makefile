#
#compile the simple event library 'ae'
#########################################

CC=clang
CFLAGS=-I./lib
CLFAGS+=-g

.PHONY:all
all:libae.a

libae.a:
	@make -C ./lib 

test:tcpServer
tcpServer:main.c lib/libae.a
	$(CC) $(CFLAGS) -o tcpServer main.c lib/libae.a

.PHONY:clean
clean:
	@make -C ./lib clean
	rm -rf *.o tcpServer








