#include <cmath>

#include "sync_file.h"

SyncFileTool::SyncFileTool() {}

SyncFileTool::~SyncFileTool()
{
    file_os_.flush();
    file_os_.close();
}

int64_t SyncFileTool::GetFileSize()
{
    int64_t len = 0;
    if(file_os_.is_open() )
    {
        file_os_.seekg(0, std::ios::end );
        len = file_os_.tellg();
        file_os_.seekg(0, std::ios::beg);
    }
    return len;
}
bool SyncFileTool::InitRead(char file_name[], int64_t len)
{
    memset(file_name_, 0, sizeof(file_name_));
    memcpy(file_name_, file_name, len);
    file_os_.open(file_name_, std::ios::in | std::ios::binary);
    return file_os_.is_open();
}
bool SyncFileTool::InitWrite(char file_name[], int64_t len)
{
    memset(file_name_, 0, sizeof(file_name_));
    memcpy(file_name_, file_name, len);
    file_os_.open(file_name_, std::ios::out | std::ios::binary);
    return file_os_.is_open();
}
bool SyncFileTool::ReadFile(char buff[], int64_t* len)
{
    // // 将文件指针移动到文件开头
    // file_os_.seekg(0, std::ios::beg);
    if (file_os_.is_open()) {
        file_os_.read(buff, *len);
        *len = file_os_.gcount();
    } else {
        *len = 0;
    }
    return *len;
}
bool SyncFileTool::WriteFile(char buff[], int64_t* len)
{
    // // 将文件指针移动到文件开头
    // file_os_.seekg(0, std::ios::beg);
    if (file_os_.is_open()) {
        file_os_.write(buff, *len);
    }
    return true;
}

bool SyncFileTool::SyncSendFile(int socket_fd, char filename[], int64_t len)
{
    InitRead(filename, len);
    int64_t file_split_len = 2048;
    int64_t file_len = GetFileSize();
    int64_t file_infos[2] = {0};
    file_infos[0] = file_len;
    file_infos[1] = file_split_len;
    //  发送文件长度
    SendBuff(socket_fd, (char*)file_infos, sizeof(file_infos));

    //  文件分组
    int file_arr = std::ceil((double)file_len/file_split_len);
    cout << "--->sync file name send:" << filename << ", size:" << file_len << "," << file_split_len << ":-->" << file_arr << endl;
    char buff[file_split_len] = {0};
    int64_t buff_len = file_split_len;
    int64_t send_index = 0;
    int64_t sum_index = 0;
    //  发送文件的内容
    while(file_arr--) {
        ++send_index;
        ReadFile(buff, &buff_len);
        SendBuff(socket_fd, buff, buff_len);
        // cout << "--->sync send size:" << buff_len << "," << sizeof(buff_len)<< endl;

        sum_index += buff_len;
        buff_len = file_split_len;
        memset(buff, 0, file_split_len);
    }
    cout << "--->send_index:" << send_index << ", sum:" << sum_index<< endl;
    return true;
}

bool SyncFileTool::SyncRecvFile(int socket_fd, char filename[], int64_t len)
{
    char file_buff[256] = {0};
    sprintf(file_buff, "%s.copy", filename);
    InitWrite(file_buff, strlen(file_buff));
    int64_t file_len = 0;
    int64_t file_split_len = 1;
    int64_t file_infos[2] = {0};

    SendBuff(socket_fd, filename, len);

    RecvBuff(socket_fd, (char*)file_infos);

    file_len = file_infos[0];
    file_split_len = file_infos[1];

    //  文件分组
    int file_arr = std::ceil((double)file_len/file_split_len);
    cout << "--->sync file name recv size:" << file_len << "," << file_split_len << ":-->" << file_arr << endl;
    char buff[file_split_len] = {0};
    int64_t buff_len = file_split_len;
    int64_t recv_index = 0;
    int64_t sum_index = 0;
    //  接受文件的内容
    while(file_arr--) {
        ++recv_index;
        buff_len = RecvBuff(socket_fd, buff);
        // cout << "--->sync recv size:" << buff_len << "," << sizeof(buff_len)<< endl;
        sum_index += buff_len;
        WriteFile(buff, &buff_len);

        buff_len = file_split_len;
        memset(buff, 0, file_split_len);
    }
    cout << "--->recv_index:" << recv_index << ", sum:" << sum_index<< endl;

    return true;
}