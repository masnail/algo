#include "server.h"

using namespace std;

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
    //  初始化线程池
    pool.addThread(pool_size);

    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    bzero(&serv_adr, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(server_port);

    if (bind(serv_sock, (struct sockaddr *)&serv_adr, sizeof(serv_adr)) == -1)
    {
        cout << "Bind error," << errno << ": " << strerror(errno) << endl;
        return false;
    }

    if (listen(serv_sock, 5) == -1)
    {
        cout << "Listen error" << errno << ": " << strerror(errno) << endl;
        return false;
    }

    epfd = epoll_create(EPOLL_SIZE); // 可以忽略这个参数，填入的参数为操作系统参考
    ep_events = (struct epoll_event *)malloc(sizeof(struct epoll_event) * EPOLL_SIZE);

    event.events = EPOLLIN; // 需要读取数据的情况
    event.data.fd = serv_sock;
    // 例程epfd 中添加文件描述符 serv_sock，目的是监听 enevt 中的事件
    epoll_ctl(epfd, EPOLL_CTL_ADD, serv_sock, &event);

    //  触发epoll
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
                // fcntl(clnt_sock, F_SETFL, fcntl(clnt_sock, F_GETFL, 0) | O_NONBLOCK);//非阻塞设置
                event.events = EPOLLIN | EPOLLET;  //  设置为边缘触发
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

                    char cmd[256] = {0};
                    int64_t len = RecvBuff(socket_fd, cmd);
                    if(len > 0) {
                        // SyncSendFile(socket_fd, filename, len);
                        pool.AddTask(socket_fd, [&, socket_fd](){
                            SyncFileTool file;
                            file.SyncSendFile(socket_fd, cmd, len);
                        });
                    } else {
                        epoll_ctl(epfd, EPOLL_CTL_DEL, socket_fd, &event);
                        close(socket_fd);
                        pool.delCloseFD(socket_fd);
                        cout << "closed client:" << socket_fd << endl;
                    }
                }
            }
        }
    }
}
