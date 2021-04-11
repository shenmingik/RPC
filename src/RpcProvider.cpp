#include "RpcProvider.hpp"
#include "RpcApplication.hpp"

using namespace std;
using namespace placeholders;

/*
service_name => service 描述
                service* 记录服务对象
                method_name => 对于method发方法对象
*/

//框架提供给外部使用的，可以发布rpc方法的函数接口
void RpcProvider::notify_service(google::protobuf::Service *service)
{
    ServiceInfo service_info;
    //获取服务对象的描述信息
    const google::protobuf::ServiceDescriptor *service_desc = service->GetDescriptor();

    //获取服务对象名字
    string service_name = service_desc->name();
    //获取服务对象方法的数量
    int method_nums = service_desc->method_count();

    //cout << "service name: " << service_name << endl;
    for (int i = 0; i < method_nums; ++i)
    {
        //获取服务对象指定下标的服务的描述
        const google::protobuf::MethodDescriptor *method_desc = service_desc->method(i);
        string method_name = method_desc->name();
        //cout << "method name: " << method_name << endl;
        //插入信息
        service_info.method_map_.insert({method_name, method_desc});
    }
    service_info.service_ = service;
    service_map_.insert({service_name, service_info});
}

//启动rpc服务节点，开始提供rpc远程网络调用服务
void RpcProvider::run()
{
    //获取ip和port
    string ip = RpcApplication::get_instance().get_configure().find_load("rpcserver_ip");
    uint16_t port = atoi(RpcApplication::get_instance().get_configure().find_load("rpcserver_port").c_str());
    //cout << ip << ":" << port << endl;
    InetAddress address(ip, port);

    //创建tcpserver对象
    TcpServer server(&eventloop_, address, "RpcProvider");
    //绑定链接回调和消息读写回调方法
    server.setConnectionCallback(bind(&RpcProvider::on_connection, this, _1));
    server.setMessageCallback(bind(&RpcProvider::on_message, this, _1, _2, _3));

    //设置muduo库的线程数量
    server.setThreadNum(4);

    //启动网络服务
    server.start();
    eventloop_.loop();
}

//新socket连接的回调
void RpcProvider::on_connection(const TcpConnectionPtr &conn)
{
}

//已建立连接的读写事件回调
void RpcProvider::on_message(const TcpConnectionPtr &conn, Buffer *buffer, Timestamp stamp)
{
}