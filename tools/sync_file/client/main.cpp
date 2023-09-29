#include "epoll_socket/client.h"
#include "file_operation/sync_file.h"

#define SERVER_IP "127.0.0.1" // 指定服务端的IP
#define SERVER_PORT 9190      // 指定服务端的port

int main(int argc, char* argv[])
{
    ClientImpl client(SERVER_PORT, SERVER_IP);
    if(!client.InitClient()) return -1;
    if(argc >= 2) {
        for(int i = 1; i < argc; ++i) {
            SyncFileTool file;
            file.SyncRecvFile(client.getSocketFD(), argv[i], strlen(argv[i]));
            // client.SendBuff(argv[i], strlen(argv[i]));
            cout << "---> get file:" << argv[i] << endl;
            // client.SyncRecvFile(argv[i], strlen(argv[i]));
        }
    } else {
        char file_name[] = "test";
        SyncFileTool file;
        file.SyncRecvFile(client.getSocketFD(), file_name, strlen(file_name));
        // client.SendBuff(file_name, strlen(file_name));
        // client.SyncRecvFile(file_name, strlen(file_name));
    }
    return 0;
}