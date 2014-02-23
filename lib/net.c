#include "ae.h"
#include "net.h"
#include "anet.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

/* Anti-warning macro... */
#define _NOTUSED(V) ((void) V)

void receiveUdpPacketHandler(aeEventLoop* el,int fd,void* privdata,int mask)
{
	assert(privdata != NULL && el != NULL);
	_NOTUSED(mask);
	uint8_t* packet = (uint8_t*)privdata;
	fprintf(stderr,"packet: %s\n",(char*)packet);
}

tcpClient_t* createClient(aeEventLoop* el,int fd,aeFileProc* read_proc,aeFileProc* write_proc)
{
	assert(fd >= 0);

	tcpClient_t* c = (tcpClient_t*)zcalloc(sizeof(tcpClient_t));
	if(c == NULL) {
		return NULL;
	}

	anetNonBlock(NULL,fd);
	anetTcpNoDelay(NULL,fd);
	
	c->fd = fd;
	c->ctime = c->last_interaction = time(NULL);
	c->read_proc = read_proc;
	c->write_proc = write_proc;

	return c;
}

int handleClientWriteEvent(aeEventLoop* el,tcpClient_t* c)
{
	return aeAddWriteEvent(el,c->fd,c->write_proc,c);
}

int handleClientReadEvent(aeEventLoop* el,tcpClient_t* c)
{
	return aeAddReadEvent(el,c->fd,c->read_proc,c);
}

void freeClient(aeEventLoop* el,tcpClient_t* c)
{
	assert(c != NULL);

	aeDeleteReadEvent(el,c->fd);
	aeDeleteWriteEvent(el,c->fd);

	if(c->inBuffer != NULL) {
		asBufferDestroy(c->inBuffer);
	}

	if(c->outBuffer != NULL) {
		asBufferDestroy(c->outBuffer);
	}

	close(c->fd);
	zfree(c);
}
