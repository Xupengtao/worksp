//
//  Hello World 客户端
//  连接REQ套接字至 tcp://localhost:5555
//  发送Hello给服务端，并接收World
//
#include <zmq.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>

int main(void)
{
    void *ctx = zmq_ctx_new();
    std::cout << "正在连接至hello world服务端..." << std::endl;
    void *requester = zmq_socket(ctx, ZMQ_REQ);
    // zmq_connect(requester, "ipc:///tmp/tester");
    zmq_connect(requester, "tcp://192.168.1.21:5550");

    int request_nbr;
    for (request_nbr = 0; request_nbr != 10; request_nbr++)
    {
        zmq_msg_t request;
        zmq_msg_init_size(&request, 5);
        memcpy(zmq_msg_data(&request), "Hello", 5);
        std::cout << "正在发送 Hello " << request_nbr << "..." << std::endl;
        zmq_msg_send(&request, requester, 0);
        zmq_msg_close(&request);

        zmq_msg_t reply;
        zmq_msg_init(&reply);
        zmq_msg_recv(&reply, requester, 0);
        std::cout << "接收到 World " << request_nbr << "..." << std::endl;
        zmq_msg_close(&reply);
    }
    zmq_close(requester);
    zmq_term(ctx);
    return 0;
}
