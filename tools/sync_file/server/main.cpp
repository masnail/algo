#include "epoll_socket/server.h"

#define SERVER_IP "127.0.0.1" // 指定服务端的IP
#define SERVER_PORT 9190      // 指定服务端的port

int main()
{
    ServerImpl server(SERVER_PORT, SERVER_IP);
    server.InitServer();
    return 0;
}