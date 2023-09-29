#include "epoll_socket.h"

using namespace std;

EpollSocket::EpollSocket() {}
EpollSocket::~EpollSocket() {}

int EpollSocket::RecvSocket(int socket_fd, char buf[], int64_t dataSize)
{
    //循环接收
    int64_t recvNums = 0;//目前收到的数据
    int64_t recvRenainNums = dataSize;//剩余要接收的数据
    while (1){
        int64_t bytesRead = recv(socket_fd, &buf[recvNums], recvRenainNums, 0);
        if (bytesRead == recvRenainNums){
               return 0;//发送完成
        }
        else if (bytesRead > 0){
              recvNums += bytesRead;
              recvRenainNums -= bytesRead;
              continue;
        }
        else if ((bytesRead < 0) && (errno == EAGAIN)) {
            continue;
        }
        else{
            return -1;
        }
    }
    return 1;
}

int EpollSocket::SendSocket(int socket_fd, char buf[], int64_t dataSize)
{
    //循环发送
    int64_t sendNums = 0;//目前发送的数据
    int64_t sendRemainNums = dataSize;//剩余要发送的数据
    while(1)
    {
        int64_t bytesSend = send(socket_fd, &buf[sendNums], sendRemainNums, 0);
        if(bytesSend == sendRemainNums) {
              return 0;
        }
        else if(bytesSend > 0){
              sendNums += bytesSend;
              sendRemainNums -= bytesSend;
              continue;
        }
        else if((bytesSend < 0)&&(errno == 11)){
              continue;
        }
        else{
              return -1;
        }
    }
    return 1;
}

int64_t EpollSocket::RecvBuff(int socket_fd, char buff[])
{
    /* 先接受长度，再根据长度进行内容接受*/
    int64_t recv_len = 0;

    int ret_len = RecvSocket(socket_fd, (char*)&recv_len, sizeof(recv_len));
    int ret_buff = RecvSocket(socket_fd, buff, recv_len);

    return recv_len;
}

bool EpollSocket::SendBuff(int socket_fd, char buff[], int64_t len)
{
    /* 先发送长度，再根据长度进行内容发送*/
    int64_t send_len = len;

    int ret_len = SendSocket(socket_fd, (char*)&send_len, sizeof(send_len));
    int ret_buff = SendSocket(socket_fd, buff, send_len);
    return (ret_buff >= 0) && (ret_len >= 0);
}