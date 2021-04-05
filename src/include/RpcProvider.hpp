#pragma once
#include <google/protobuf/service.h>
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/TcpConnection.h>
#include <muduo/net/Buffer.h>
#include <muduo/base/Timestamp.h>

#include <memory>

using namespace muduo::net;
using namespace muduo;
using namespace std;

//框架提供的专门负责发布rpc服务的网络框架类
class RpcProvider
{
public:
    //框架提供给外部使用的，可以发布rpc方法的函数接口
    void notify_service(google::protobuf::Service *service);

    //启动rpc服务节点，开始提供rpc远程网络调用服务
    void run();

private:
    //新socket连接的回调
    void on_connection(const TcpConnectionPtr& conn);

    //已建立连接的读写事件回调
    void on_message(const TcpConnectionPtr& conn,Buffer* buffer,Timestamp stamp);
private:
    //unique_ptr<TcpServer> serverPtr_;

    EventLoop eventloop_;

};