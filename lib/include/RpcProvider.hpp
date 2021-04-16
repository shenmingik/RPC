#pragma once
#include <google/protobuf/service.h>
#include <google/protobuf/descriptor.h>
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/TcpConnection.h>
#include <muduo/net/Buffer.h>

#include <unordered_map>
#include <string>
#include <iostream>
#include <functional>

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
    void on_connection(const TcpConnectionPtr &conn);

    //已建立连接的读写事件回调
    void on_message(const TcpConnectionPtr &conn, Buffer *buffer, Timestamp stamp);

    //Clouser的回调操作，用于序列化rpc的响应和网络发送
    void send_rpc_response(const TcpConnectionPtr &conn,google::protobuf::Message* reponse);

private:
    //服务类型信息
    struct ServiceInfo
    {
        //保存服务对象
        google::protobuf::Service *service_;
        //保存服务方法：    <服务名，指向方法指针>
        unordered_map<string, const google::protobuf::MethodDescriptor *> method_map_;
    };

private:
    /*unique_ptr<TcpServer> serverPtr_;*/
    EventLoop eventloop_;
    //存储服务信息
    unordered_map<string, ServiceInfo> service_map_;
};