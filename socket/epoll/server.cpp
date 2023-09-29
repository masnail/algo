#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <sys/socket.h>
#include <sys/unistd.h>
#include <sys/types.h>
#include <sys/errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/epoll.h>

#include <iostream>
#include <string>

#include "ThreadPool.h"

#include "echo.pb.h"

using namespace std;

#define EPOLL_SIZE 50
#define SERVER_IP "127.0.0.1" // 指定服务端的IP
#define SERVER_PORT 9190      // 指定服务端的port

class ServerImpl
{
private:
    /* data */
    string server_ip;
    int server_port;

    // socket
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_adr;
    struct sockaddr_in clnt_adr;
    socklen_t adr_sz;

    // epoll
    struct epoll_event *ep_events;
    struct epoll_event event;
    int epfd;
    int event_cnt;

    //线程池
    ThreadPool pool;

public:
    ServerImpl(int server_port, string server_ip);
    ~ServerImpl();
    bool InitServer(int pool_size = 4);
    bool RecvBuff(int socket_fd);
    bool SendBuff(int socket_fd, int32_t data);

private:
    void ExecEpoll();
};

ServerImpl::ServerImpl(int server_port, string server_ip):server_ip(server_ip),server_port(server_port)
{
}

ServerImpl::~ServerImpl()
{
    close(serv_sock);
    close(epfd);
}
bool ServerImpl::InitServer(int pool_size)
{
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    bzero(&serv_adr, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(server_port);

    if (bind(serv_sock, (struct sockaddr *)&serv_adr, sizeof(serv_adr)) == -1)
        cout << "Bind error" << errno << ": " << strerror(errno) << endl;

    if (listen(serv_sock, 5) == -1)
        cout << "Listen error" << errno << ": " << strerror(errno) << endl;

    epfd = epoll_create(EPOLL_SIZE); // 可以忽略这个参数，填入的参数为操作系统参考
    ep_events = (struct epoll_event *)malloc(sizeof(struct epoll_event) * EPOLL_SIZE);

    event.events = EPOLLIN; // 需要读取数据的情况
    event.data.fd = serv_sock;
    // 例程epfd 中添加文件描述符 serv_sock，目的是监听 enevt 中的事件
    epoll_ctl(epfd, EPOLL_CTL_ADD, serv_sock, &event);

    //  初始化线程池
    pool.addThread(pool_size);
    ExecEpoll();
    return true;
}

void ServerImpl::ExecEpoll()
{
    while (1)
    {
        event_cnt = epoll_wait(epfd, ep_events, EPOLL_SIZE, -1); // 获取改变了的文件描述符，返回数量
        if (event_cnt == -1)
        {
            cout << "epoll_wait() error" << endl;
            break;
        }

        for (int i = 0; i < event_cnt; ++i)
        {
            if (ep_events[i].data.fd == serv_sock) // 客户端请求连接时
            {
                adr_sz = sizeof(clnt_adr);
                clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_adr, &adr_sz);
                event.events = EPOLLIN;
                event.data.fd = clnt_sock; // 把客户端套接字添加进去
                epoll_ctl(epfd, EPOLL_CTL_ADD, clnt_sock, &event);
                cout << "connected client:" << clnt_sock << endl;
            }
            else // 是客户端套接字时
            {
                // protobuf
                if (ep_events[i].events & EPOLLIN)
                {
                    int socket_fd = ep_events[i].data.fd;
                    bool ret = RecvBuff(socket_fd);
                }

                //  bytes
                // if (ep_events[i].events & EPOLLIN) {
                //     int socket_fd = ep_events[i].data.fd;
                //     int buf[BUF_SIZE];
                //     int str_len = read(socket_fd,buf, sizeof(int)*2);
                //     if(str_len == 0)
                //     {
                //         epoll_ctl(epfd, EPOLL_CTL_DEL, socket_fd, &event);
                //         close(socket_fd);
                //         pool.delCloseFD(socket_fd);
                //         cout << "closed client:" << socket_fd << endl;
                //     } else {
                //         pool.AddTask(socket_fd, [buf, socket_fd, epfd, &event, str_len]()
                //         {

                //             cout << "server id >>>>[" << socket_fd << "] recv:" << char(buf[0]+'A') << ":" << buf[1]<< endl;

                //             write(socket_fd,buf, sizeof(int)*2);
                //         }
                //         );
                //     }
                // }
            }
        }
    }
}

bool ServerImpl::RecvBuff(int socket_fd)
{
    Protobuf::Client client;
    int data_len = 0;
    int str_len = read(socket_fd, &data_len, sizeof(data_len));
    char buf[data_len];
    str_len = read(socket_fd, buf, sizeof(buf));
    if (str_len == 0)
    {
        epoll_ctl(epfd, EPOLL_CTL_DEL, socket_fd, &event);
        close(socket_fd);
        pool.delCloseFD(socket_fd);
        cout << "closed client:" << socket_fd << endl;
    }
    else
    {
        client.ParseFromArray(buf, str_len);
        pool.AddTask(socket_fd, [&, client, socket_fd, str_len]() {
            SendBuff(socket_fd, client.data());
        });
    }
    return str_len;
}

bool ServerImpl::SendBuff(int socket_fd, int32_t data)
{
    Protobuf::Server server;
    server.set_server_id(socket_fd);
    server.set_data(data);
    write(socket_fd, &server, sizeof(server)); 
    return true;
}


int main()
{
    ServerImpl server(SERVER_PORT, SERVER_IP);
    server.InitServer();
    return 0;
}
