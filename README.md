libae: a simple event library from Redis database
============

@author: [qudreams]

Note:
	
	The library drive from Redis database source.please respect the author copyright of Redis.you can use the library as you want.
	The library just supports epoll and select event-trigger,and I will add other event-trigger in future.
	It also doesn't support IPv6 address;if you want to use IPv6,you can add by your self; 

## About
	source files of the libae is in directory lib,you can complie the library to use following directive:
		make
	It will generate a static library file 'libae.a' in directory 'lib'.
	You can find the example in the source file example.c.
	The example is a tcp server listening port 4096.
	You can complile the example to use following directive.
		make test
