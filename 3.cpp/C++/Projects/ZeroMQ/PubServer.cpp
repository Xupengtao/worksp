//
//  气象信息更新服务
//  绑定PUB套接字至tcp://*:5556端点
//  发布随机气象信息
//
#include <zmq.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#define randof(num) (int)((float)(num)*random() / (RAND_MAX + 1.0))

static int s_send(void *socket, char *string)
{
    int size = zmq_send(socket, string, strlen(string), 0);
    return size;
}

int main(void)
{
    void *ctx = zmq_ctx_new();
    void *PubSocket = zmq_socket(ctx, ZMQ_PUB);
    // zmq_bind (PubSocket, "ipc://weather.ipc");
    int rc = zmq_bind(PubSocket, "tcp://192.168.1.21:5559");

    srandom((unsigned)time(NULL));
    while (1)
    {
        int zipcode, temperature, relhumidity;
        zipcode = randof(100000);
        temperature = randof(215) - 80;
        relhumidity = randof(50) + 10;

        char update[20];
        sprintf(update, "%05d %d %d", zipcode, temperature, relhumidity);
        s_send(PubSocket, update);
    }
    zmq_close(PubSocket);
    zmq_term(ctx);
    return 0;
}