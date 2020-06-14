#include "../ZmqSocket.hpp"
#include <unistd.h>

#define randof(num)  (int) ((float) (num) * random () / (RAND_MAX + 1.0))

static void s_sleep (int msecs)
{
#if (defined (WIN32))
    Sleep (msecs);
#else
    struct timespec t;
    t.tv_sec = msecs / 1000;
    t.tv_nsec = (msecs % 1000) * 1000000;
    nanosleep(&t, NULL);
#endif
}

static int64_t s_clock (void)
{
#if (defined (WIN32))
    SYSTEMTIME st;
    GetSystemTime (&st);
    return (int64_t) st.wSecond * 1000 + st.wMilliseconds;
#else
    struct timeval tv;
    gettimeofday (&tv, NULL);
    return (int64_t) (tv.tv_sec * 1000 + tv.tv_usec / 1000);
#endif
}

int main(int argc, char** args)
{
    if(argc == 3)
    {
        string argStr1 = chartostring(args[1]);
        string argStr2 = chartostring(args[2]);
        char *recvBuffer;
        if(argStr1 == "0")
        {
            COUTS("as Rep");
            _ZmqSocket ZmqSocket(ZmqTcpIp, argStr2, ZmqRep, ZmqBind);
            while (1)
            {
                recvBuffer = ZmqSocket.Recv();
                COUT(recvBuffer);
                sleep(1);
                ZmqSocket.Send("World!");
            }
        }
        else if(argStr1 == "1")
        {
            COUTS("as Req");
            _ZmqSocket ZmqSocket(ZmqTcpIp, argStr2, ZmqReq, ZmqConnect);
            while (1)
            {
                ZmqSocket.Send("Hello!");
                sleep(1);
                recvBuffer = ZmqSocket.Recv();
                COUT(recvBuffer);
            }
        }
        else if(argStr1 == "2")
        {
            COUTS("as Pub");
            _ZmqSocket ZmqSocket(ZmqTcpIp, argStr2, ZmqPub, ZmqBind);
            while (1)
            {
                ZmqSocket.Send("Hello!");
                sleep(1);
            }
        }
        else if(argStr1 == "3")
        {
            COUTS("as Sub");
            _ZmqSocket ZmqSocket(ZmqTcpIp, argStr2, ZmqSub, ZmqConnect);
            ZmqSocket.Filter("Hello");
            while (1)
            {
                recvBuffer = ZmqSocket.Recv();
                COUT(recvBuffer);
                sleep(1);
            }
        }
        else if(argStr1 == "4")
        {
            COUTS("as Push");
            _ZmqSocket ZmqSocket(ZmqTcpIp, argStr2, ZmqPush, ZmqBind);
            _ZmqSocket sinkSocket(ZmqTcpIp, "localhost:5568", ZmqPush, ZmqConnect);
            COUTS("准备好后按任意键开始: ");
            cin.get();
            COUTS("分配任务...");
            sinkSocket.Send("0");
            srandom((unsigned)time(NULL));
            int taskCnt;
            int totalMs = 0;
            for(taskCnt = 0; taskCnt < 100; taskCnt++)
            {
                int workload;
                workload = randof(100) + 1;
                totalMs  += workload;
                char chStr[10];
                sprintf(chStr, "%d", workload);
                ZmqSocket.Send(chStr);
            }
            COUTS("预计执行时间: ", totalMs, "毫秒");
            sleep(1);
        }
        else if(argStr1 == "5")
        {
            COUTS("as Pull");
            _ZmqSocket ZmqSocket(ZmqTcpIp, argStr2, ZmqPull, ZmqConnect);
            _ZmqSocket sendSocket(ZmqTcpIp, "localhost:5568", ZmqPush, ZmqConnect);
            while(1)
            {
                char *chStr = ZmqSocket.Recv();
                fflush(stdout);
                COUTS("", chStr);
                s_sleep(atoi(chStr));
                sendSocket.Send("");
            }
        }
        else if(argStr1 == "6")
        {
            COUTS("as Pull Collect");
            _ZmqSocket ZmqSocket(ZmqTcpIp, "*:5568", ZmqPull, ZmqBind);
            char *chStr = ZmqSocket.Recv();
            int64_t startTime = s_clock();
            int taskCnt;
            for(taskCnt = 0; taskCnt < 100; taskCnt++)
            {
                chStr = ZmqSocket.Recv();
                if((taskCnt % 10) == 0)
                {
                    COUTS(":");
                }
                else
                {
                    COUTS(".");
                }
                fflush(stdout);
            }
            COUTS("执行时间: ", (int)(s_clock() - startTime), "毫秒");
        }
    }
    else
    {
        ;
    }
}