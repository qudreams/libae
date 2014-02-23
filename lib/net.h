#ifndef net_h
#define net_h

#include <stdint.h>
#include "zmalloc.h"
#include "ae.h"
#include "asBuffer.h"

#define _TCP_OUTPUT_BUF_LEN (16*1024) /*16kb tcp output buffer*/
#define _TCP_INPUT_BUF_LEN (16*1024) /*16kb tcp input buffer*/

struct aeEventLoop;
typedef struct {
    int fd;
    time_t ctime;
	time_t last_interaction;
	asBuffer* inBuffer;
    asBuffer* outBuffer;
	aeFileProc* read_proc;
	aeFileProc* write_proc;
}tcpClient_t;

tcpClient_t* createClient(aeEventLoop* el,int fd,aeFileProc* read_proc,aeFileProc* write_proc);
void freeClient(aeEventLoop* el,tcpClient_t* c);
int handleClientReadEvent(aeEventLoop* el,tcpClient_t* c);
int handleClientWriteEvent(aeEventLoop* el,tcpClient_t* c);

#endif
