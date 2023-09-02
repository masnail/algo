#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <sys/socket.h>
#include <sys/unistd.h>
#include <sys/types.h>
#include <sys/errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <iostream>
#include <string>
#include <random>
#include <string>

#include "echo.pb.h"

using namespace std;

#define SERVER_IP "127.0.0.1" // 指定服务端的IP
#define SERVER_PORT 9190      // 指定服务端的port


class ClientImpl
{
private:
    /* data */
    int server_port;
    string server_ip;
    int client_id;

    int sock;
    struct sockaddr_in serv_adr;
public:
    ClientImpl(int server_port, string server_ip);
    ~ClientImpl();
    void InitClient();
    bool RecvBuff(int socket_fd);
    bool SendBuff(int socket_fd);
};

ClientImpl::ClientImpl(int server_port, string server_ip):server_ip(server_ip),server_port(server_port)
{
    std::random_device rd;
    std::default_random_engine eng(rd());
    std::uniform_int_distribution<int> distr(0, 25);
    client_id = distr(eng);
}

ClientImpl::~ClientImpl()
{
    close(sock);
}

void ClientImpl::InitClient()
{
    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1)
        cout << "socket() error" << endl;

    bzero(&serv_adr, sizeof(serv_adr));

    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = inet_addr(SERVER_IP);
    serv_adr.sin_port = htons(SERVER_PORT);

    if (connect(sock, (struct sockaddr *)&serv_adr, sizeof(serv_adr)) == -1)
        cout << "connect() error: " << errno << " " << strerror(errno) << endl;
    else
        cout << "connected[" << char(client_id + 'A') << "]............" << endl;

    SendBuff(sock);
}

bool ClientImpl::RecvBuff(int socket_fd)
{
    Protobuf::Server server;
    read(sock, &server, sizeof(server));
    cout << "Message from server:" << server.server_id() << ":" << server.data() << endl;
    
    return true;
}

bool ClientImpl::SendBuff(int socket_fd)
{
    int index = 0;

    char pb_buff[1] = {char('A' + client_id)};

    string cli_name = pb_buff;

    Protobuf::Client client;
    client.set_client_id(cli_name);
    while (1)
    {
        // protobuf
        cout << "Message from server:" << cli_name << endl;
        client.set_data(index++);
        string data = client.SerializeAsString();
        int len = data.length();
        write(sock, &len, sizeof(len));
        write(sock, data.c_str(), data.length());
        RecvBuff(socket_fd);

        // bytes
        // memset(message, 0, sizeof(message));
        // str_len = read(sock, message, sizeof(int)*2);
        // cout << "Message from server:" << char(message[0]+'A') << ":" << message[1] << endl;
        sleep(0.1);
    }
    return true;
}


int main()
{
    ClientImpl client(SERVER_PORT, SERVER_IP);
    client.InitClient();
    return 0;
}
