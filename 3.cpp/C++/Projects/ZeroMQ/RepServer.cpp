#include <zmq.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <iostream>

int main(void)
{
    void *ctx = zmq_ctx_new();
    void *responder = zmq_socket(ctx, ZMQ_REP);
    // int rc = zmq_bind(responder, "ipc:///tmp/tester");
    int rc = zmq_bind(responder, "tcp://192.168.1.21:5550");

    while (1)
    {
        zmq_msg_t Req;
        zmq_msg_init(&Req);
        zmq_msg_recv(&Req, responder, 0);
        std::cout<<"收到 Hello"<<std::endl;
        zmq_msg_close(&Req);

        sleep(1);

        zmq_msg_t Rep;
        zmq_msg_init(&Rep);
        zmq_msg_init_size(&Rep, 88);
        memcpy(zmq_msg_data(&Rep), " [address, contents] = subscriber.recv_multipart() print([%s] %s % (address, contents))", 88);
        zmq_msg_send(&Rep, responder, 0);
        zmq_msg_close(&Rep);
    }

    zmq_close(responder);
    zmq_ctx_term(ctx);
    return 0;
}