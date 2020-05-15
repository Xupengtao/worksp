//
//  气象信息客户端
//  连接SUB套接字至tcp://*:5556端点
//  收集指定邮编的气象信息，并计算平均温度
//
#include "zhelpers.h"

int main(int argc, char *argv[])
{
    void *ctx = zmq_ctx_new();

    printf("正在收集气象信息...\n");
    void *subscriber = zmq_socket(ctx, ZMQ_SUB);
    // zmq_connect(subscriber, "ipc://weather.ipc");
    int rc = zmq_connect(subscriber, "tcp://192.168.1.21:5559");

    char *filter = "";
    zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, filter, strlen(filter));

    int update_nbr;
    long total_temp = 0;
    int zipcode, temperature, relhumidity;
    for (update_nbr = 0; update_nbr < 100; update_nbr++)
    {
        char *string = s_recv(subscriber);
        printf("1");
        free(string);
    }

    zmq_close(subscriber);
    zmq_term(ctx);
    return 0;
}