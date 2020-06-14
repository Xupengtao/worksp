#ifndef _ZMQ_CONTENT_HPP
#define _ZMQ_CONTENT_HPP
#include <zmq.h>
#include <stdlib.h>
#include <string.h>
#include "Macro.h"
#include "Error.h"
#include "Timeit.h"
#include "XptType.h"
#include "Singleton.hpp"

namespace _Zmq
{
    enum _CommProtocol
    {
        ZmqThread,
        ZmqProcess,
        ZmqTcpIp
    };
    enum _MsgProtocol
    {
        RepReq,
        PubSub,
        PushPull
    };
    enum _ZmqType
    {
        ZmqRep,
        ZmqReq,
        ZmqPub,
        ZmqSub,
        ZmqPush,
        ZmqPull
    };
    enum _ZmqBindType
    {
        ZmqBind,
        ZmqConnect
    };
};
using namespace _Zmq;

class S_ZmqContext : public Singleton<S_ZmqContext>
{
private:
    void *ctx;
public:
    S_ZmqContext(token)
    {
        std::cout<<"zmq_ctx_new called!"<<std::endl;
        ctx = zmq_ctx_new();
    }
    ~S_ZmqContext()
    {
        std::cout<<"constructor called!"<<std::endl;
        zmq_term(ctx);
    }
    S_ZmqContext(const S_ZmqContext&) = delete;
    S_ZmqContext operator = (const S_ZmqContext&) = delete;
    void* GetCtx() const {return ctx;}
};

class _ZmqSocket
{
    _CommProtocol   CommProtocol = ZmqTcpIp;
    _MsgProtocol    MsgProtocol  = RepReq;
    _ZmqType        ZmqType      = ZmqRep;
    _ZmqBindType    ZmqBindType  = ZmqBind;
    S_ZmqContext&   ZmqContext;
    void            *ZmqSocket   = NULL;
    string          ConnectPort  = "";
    bool            Connected    = false;
    char            *RecvBuffer  = NULL;
    CINT            RecvBufferSize;
public:
    _ZmqSocket(_CommProtocol CommProtocol_, string ConnectPort_, _ZmqType ZmqType_, _ZmqBindType ZmqBindType_ = ZmqBind, int RecvBufferSize_ = 1024)
              : ZmqContext(S_ZmqContext::get_instance()), RecvBufferSize(RecvBufferSize_)
    {
        RecvBuffer = new char[RecvBufferSize];
        Init(CommProtocol_, ConnectPort_, ZmqType_, ZmqBindType_);
    }
    int Init(_CommProtocol CommProtocol_, string ConnectPort_, _ZmqType ZmqType_, _ZmqBindType ZmqBindType_ = ZmqBind)
    {
        CommProtocol = CommProtocol_;
        ConnectPort  = ConnectPort_;
        ZmqType      = ZmqType_;
        ZmqBindType  = ZmqBindType_;
        Connected    = false;
        int ConnectRtn = -1;
        string StrAddr;
        CCHAR *addr = NULL;
        if(ZmqSocket != NULL)
        {
            zmq_close(ZmqSocket);
        }
        switch(ZmqType)
        {
        case ZmqRep:
            MsgProtocol = RepReq;
            ZmqSocket = zmq_socket(ZmqContext.GetCtx(), ZMQ_REP);
            break;
        case ZmqReq:
            MsgProtocol = RepReq;
            ZmqSocket = zmq_socket(ZmqContext.GetCtx(), ZMQ_REQ);
            break;
        case ZmqPub:
            MsgProtocol = PubSub;
            ZmqSocket = zmq_socket(ZmqContext.GetCtx(), ZMQ_PUB);
            break;
        case ZmqSub:
            MsgProtocol = PubSub;
            ZmqSocket = zmq_socket(ZmqContext.GetCtx(), ZMQ_SUB);
            break;
        case ZmqPush:
            MsgProtocol = PushPull;
            ZmqSocket = zmq_socket(ZmqContext.GetCtx(), ZMQ_PUSH);
            break;
        case ZmqPull:
            MsgProtocol = PushPull;
            ZmqSocket = zmq_socket(ZmqContext.GetCtx(), ZMQ_PULL);
            break;
        default:
            break;
        }
        switch(CommProtocol)
        {
        case ZmqThread:
            break;
        case ZmqProcess:
            StrAddr = "ipc:///"+ConnectPort;
            addr    = stringtochar(StrAddr);
            break;
        case ZmqTcpIp:
            StrAddr = "tcp://"+ConnectPort;
            addr    = stringtochar(StrAddr);
            break;
        default:
            break;
        }
        switch(ZmqBindType)
        {
            case ZmqBind:
                ConnectRtn = zmq_bind(ZmqSocket, addr);
                break;
            case ZmqConnect:
                ConnectRtn = zmq_connect(ZmqSocket, addr);
                break;
            default:
                break;
        }
        if(ConnectRtn != -1)
        {
            Connected = true;
        }
        else
        {
            string frontErrMsg = "Socket connect failed! IP/Port:";
            string errMsg = frontErrMsg + addr;
            ERRORMSG(errMsg);
        }
        return ConnectRtn;
    }
    virtual ~_ZmqSocket()
    {
        if(ZmqSocket != NULL)
        {
            zmq_close(ZmqSocket);
        }
        delete[] RecvBuffer;
    }
    void Filter(char *filter = "")
    {
        if((ZmqType == ZmqSub) && (ZmqSocket != NULL))
        {
            zmq_setsockopt(ZmqSocket, ZMQ_SUBSCRIBE, filter, strlen(filter));
        }
    }
    inline int Send(char *string)
    {
        int size = zmq_send(ZmqSocket, string, strlen(string), 0);
        return size;
    }
    inline int SendMore(char *string) 
    {
        int size = zmq_send(ZmqSocket, string, strlen (string), ZMQ_SNDMORE);
        return size;
    }
    inline char* Recv()
    {
        int size = zmq_recv(ZmqSocket, RecvBuffer, RecvBufferSize - 1, 0);
        if(size == -1)
        {
            return NULL;
        }
        RecvBuffer[size] = '\0';
        return RecvBuffer;
    }
    inline void Dump()
    {
        int rc;
        zmq_msg_t message;
        rc = zmq_msg_init(&message);
        assert(rc == 0);
        puts("----------------------------------------");   //  Process all parts of the message
        do
        {
            int size = zmq_msg_recv(&message, ZmqSocket, 0);
            assert(size >= 0);
            char *data = (char*)zmq_msg_data(&message);     //  Dump the message as text or binary
            assert(data != 0);
            int is_text = 1;
            int char_nbr;
            for(char_nbr = 0; char_nbr < size; char_nbr++)
            {
                if((unsigned char)data[char_nbr] < 32 || (unsigned char)data[char_nbr] > 126)
                {
                    is_text = 0;
                }
            }
            printf("[%03d] ", size);
            for(char_nbr = 0; char_nbr < size; char_nbr++)
            {
                if(is_text)
                {
                    printf("%c", data[char_nbr]);
                }
                else
                {
                    printf("%02X", (unsigned char)data[char_nbr]);
                }
            }
            printf("\n");
        } while(zmq_msg_more(&message));
        rc = zmq_msg_close(&message);
        assert (rc == 0);
    }
};

#endif  // _ZMQ_CONTENT_HPP