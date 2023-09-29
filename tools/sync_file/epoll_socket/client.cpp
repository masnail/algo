#include "client.h"

using namespace std;

ClientImpl::ClientImpl(int server_port, string server_ip):server_ip(server_ip),server_port(server_port)
{
    std::random_device rd;
    std::default_random_engine eng(rd());
    std::uniform_int_distribution<int> distr(0, 25);
    client_id = distr(eng);
}

ClientImpl::~ClientImpl()
{
    close(socket_fd);
}

bool ClientImpl::InitClient()
{
    socket_fd = socket(PF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1) {
        cout << "socket() error" << endl;
        return false;
    }

    bzero(&serv_adr, sizeof(serv_adr));

    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = inet_addr(server_ip.c_str());
    serv_adr.sin_port = htons(server_port);

    if (connect(socket_fd, (struct sockaddr *)&serv_adr, sizeof(serv_adr)) == -1) {
        cout << "connect() error: " << errno << " " << strerror(errno) << endl;
        return false;
    } else {
        cout << "connected  [" << char(client_id + 'A') << "]............" << endl;
    }
    return true;
}

int ClientImpl::getSocketFD()
{
    return socket_fd;
}
