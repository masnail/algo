# 简介
此代码为C++ epoll和线程池示例代码
# 编译
命令如下
## Client
```shell
 g++ client.cpp echo.pb.cc -o client -std=c++11 -lprotobuf
```
## Server
```shell
 g++ -o server server.cpp echo.pb.cc -std=c++11 -lpthread -lprotobuf
 ```