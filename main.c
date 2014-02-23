#include "ae.h"
#include "anet.h"
#include "net.h"
#include "asBuffer.h"
#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>


static void process_input_buffer(aeEventLoop* el,tcpClient_t* c) {
    assert(el != NULL && c != NULL);

    asBuffer* inBuffer = c->inBuffer;
    asBuffer* outBuffer = c->outBuffer;
    size_t len = asBufferLen(inBuffer);

    if(len > 0)
    {
        if(outBuffer == NULL) {
            outBuffer = asBufferCreate(_TCP_OUTPUT_BUF_LEN);
            if(outBuffer == NULL) {
                freeClient(el,c);
                return;
            }

            c->outBuffer = outBuffer;
        }

        if(len > (outBuffer->end - outBuffer->last)) {
            size_t reallocSize = asBufferLen(outBuffer) + len;
            c->outBuffer = asBufferCreate(reallocSize);
            if(c->outBuffer == NULL) {
                freeClient(el,c);
                return;
            }

            //copy data in old output buffer.
            memcpy(c->outBuffer->last,outBuffer->pos,asBufferLen(outBuffer));
            c->outBuffer->last += asBufferLen(outBuffer);
            asBufferDestroy(outBuffer);
            outBuffer = c->outBuffer;
        }

        if(handleClientWriteEvent(el,c) == AE_OK) {
            //copy data in input buffer
            memcpy(outBuffer->last,inBuffer->pos,len);
            outBuffer->last += len;
            inBuffer->pos += len;

            if(asBufferLen(inBuffer) == 0) {
                asBufferReset(inBuffer);
            }
        } else {
            freeClient(el,c);
        }
    }
}

static void read_packet_from_client(aeEventLoop* el,int fd,void* privdata,int mask) {
    tcpClient_t* c = (tcpClient_t*)privdata;
    asBuffer* inBuffer = c->inBuffer;

    if(inBuffer == NULL) {
        inBuffer = asBufferCreate(_TCP_INPUT_BUF_LEN);
        if(inBuffer == NULL) {
            freeClient(el,c);
            return;
        }
        c->inBuffer = inBuffer;
    } else {
        if(inBuffer->pos != inBuffer->start) {
            size_t len = asBufferLen(inBuffer);
            memmove(inBuffer->start,inBuffer->pos,len);
            inBuffer->pos = inBuffer->start;
            inBuffer->last = inBuffer->pos + len;
        }
    }

    int nread = read(fd,inBuffer->last,inBuffer->end - inBuffer->last);
    if(nread < 0) {
        if(errno != EAGAIN)
        {
            freeClient(el,c);
            fprintf(stderr,"error: %s\n",strerror(errno));
            return;
        }
    } else if(nread == 0) {
        freeClient(el,c);
        fprintf(stderr,"Client closed connections\n");
        return;
    } else {/*we read something*/
        inBuffer->last += nread;
        process_input_buffer(el,c);
    }
}


static void send_reply_to_client(aeEventLoop* el,int fd,void* privdata,int mask) {
    int nwritten = 0,towritten = 0;
    tcpClient_t* c = (tcpClient_t*) privdata; 
    asBuffer* outBuffer = c->outBuffer;

    while(asBufferLen(outBuffer) > 0) {
        nwritten = write(c->fd,outBuffer->pos,asBufferLen(outBuffer));
        if(nwritten <= 0) {
            if(errno != EINTR) {
                break;/*may be error*/
            }
        }
        
        outBuffer->pos += nwritten;
        towritten += nwritten;
    }

    if(nwritten == -1) {
        //write error,but the errno may be EAGAIN
        if(errno == EAGAIN)
            nwritten = 0;
        else
        {
            fprintf(stderr,"Error writing to client: %s\n",strerror(errno));
            freeClient(el,c);
            return;
        }
    }

    if(towritten > 0) {
        c->last_interaction = time(NULL);
    }

    if(asBufferLen(outBuffer) == 0) {
        asBufferReset(outBuffer);
        aeDeleteWriteEvent(el,c->fd);
    }
}

void initTcpConnection(aeEventLoop* el,int fd) {
    assert(el != NULL && fd >= 0);
    tcpClient_t* c = createClient(el,fd,read_packet_from_client,send_reply_to_client);
    if(c == NULL) {
        fprintf(stderr, "failed to create client\n");
    }

    if(handleClientReadEvent(el,c) == AE_ERR) {
        fprintf(stderr, "failed to handle client write event\n");
        freeClient(el,c);
    }
}


void acceptTcpConnectionHandler(aeEventLoop *el, int fd, void *privdata, int mask) {
    int cport, cfd;
    char cip[128];

    char neterr[1024] = {0};
    cfd = anetTcpAccept(neterr, fd, cip, &cport);
    if (cfd == AE_ERR) {
        fprintf(stderr,"Accepting client connection: %s",neterr);
        return;
    }
    initTcpConnection(el,cfd);
}


int main(int argc,char* argv[])
{
    aeEventLoop* elp = aeCreateEventLoop(1024);
    if(elp == NULL)
        return -1;
    char errbuf[1024] = {0};
    int sfd = anetTcpServer(errbuf,4096,argv[1]);
    if(sfd == -1) {
        fprintf(stderr,"create tcp server error: %s\n",errbuf);
        return -1;
    }

    if(aeAddReadEvent(elp,sfd,acceptTcpConnectionHandler,NULL) == AE_ERR) {
        return -1;
    }

    aeMain(elp);
    return 0;
}
