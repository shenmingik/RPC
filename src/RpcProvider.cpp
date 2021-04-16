#include "RpcProvider.hpp"
#include "RpcApplication.hpp"
#include "RpcHeader.pb.h"
#include "RpcLogger.hpp"
#include "ZookeeperClient.hpp"

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

    //把当前rpc节点上要发布的服务全部注册到zk上面，让rpc client可以从zk上发现服务
    ZookeeperClient zk_client;
    zk_client.start();

    //在配置中心中创建节点
    for (auto &sp : service_map_)
    {
        string service_path = "/" + sp.first;
        zk_client.create(service_path.c_str(), nullptr, 0);
        for (auto &mp : sp.second.method_map_)
        {
            string method_path = service_path + "/" + mp.first;
            char method_path_data[128] = {0};
            sprintf(method_path_data, "%s:%d", ip.c_str(), port);
            //ZOO_EPHEMERAL 表示znode时候临时性节点
            zk_client.create(method_path.c_str(), method_path_data, strlen(method_path_data), ZOO_EPHEMERAL);
        }
    }

    RPC_LOG_INFO("server RpcProvider [ip: %s][port: %d]", ip.c_str(), port);
    //启动网络服务
    server.start();
    eventloop_.loop();
}

//新socket连接的回调
void RpcProvider::on_connection(const TcpConnectionPtr &conn)
{
    if (!conn->connected())
    {
        //和rpc client断开连接
        conn->shutdown();
    }
}

/*
    在框架内部，RpcPrivoder和RpcConsumer协商好之间通信用的protobuf类型
标识长度：header_size = service_name + method_name + argc_size
    service_name ==> service
    method_name ==> method
    定义proto的message类型：
        args

recv_buf = header + args
16UserServiceLogin15zhang san123456

--> header_size（4字节）+ header_str + args_str
*/

//已建立连接的读写事件回调
void RpcProvider::on_message(const TcpConnectionPtr &conn, Buffer *buffer, Timestamp stamp)
{
    // 网络上接受的远程rpc调用请求的字符流
    string recv_buf = buffer->retrieveAllAsString();

    //从字符流中读取前四个字节的内容(利用int特性),即header的长度
    uint32_t header_size = 0;
    recv_buf.copy((char *)&header_size, 4, 0);

    RPC_LOG_INFO("header size: %d", header_size);

    //根据header_size读取数据头的原始字符流
    string rpc_header_str = recv_buf.substr(4, header_size);

    //反序列化
    ikrpc::RpcHeader rpc_header;
    string service_name;
    string method_name;
    uint32_t args_size = 0;
    if (rpc_header.ParseFromString(rpc_header_str))
    {
        //数据头反序列化成功
        service_name = rpc_header.service_name();
        method_name = rpc_header.method_name();
        args_size = rpc_header.args_size();
    }
    else
    {
        //数据头反序列化失败
        RPC_LOG_ERROR("rpc header str:%s parse error!", rpc_header_str.c_str());
        return;
    }

    //解析参数
    string args_str = recv_buf.substr(4 + header_size, args_size);

    //打印调试信息
    RPC_LOG_INFO("<------------------------------------------------>");
    RPC_LOG_INFO("rpc header str: %s", rpc_header_str.c_str());
    RPC_LOG_INFO("service name: %s", service_name.c_str());
    RPC_LOG_INFO("method name: %s", method_name.c_str());
    RPC_LOG_INFO("args: %s", args_str.c_str());
    RPC_LOG_INFO("<------------------------------------------------>");

    //获取service对象和method对象
    auto service_it = service_map_.find(service_name);
    if (service_it == service_map_.end())
    {
        RPC_LOG_ERROR("%s is not exist", service_name.c_str());
        return;
    }

    auto method_it = service_it->second.method_map_.find(method_name);
    if (method_it == service_it->second.method_map_.end())
    {
        RPC_LOG_ERROR("%s::%s is not exist", service_name.c_str(), method_name.c_str());
        return;
    }

    //获取service和method对象
    google::protobuf::Service *service = service_it->second.service_;
    const google::protobuf::MethodDescriptor *method = method_it->second;

    //生成rpc方法调用的请求request 和响应response参数
    google::protobuf::Message *request = service->GetRequestPrototype(method).New();
    if (!request->ParseFromString(args_str))
    {
        RPC_LOG_ERROR("request parse error!");
        return;
    }
    google::protobuf::Message *response = service->GetResponsePrototype(method).New();

    //给callMethod方法的调用，绑定一个closure回调函数
    google::protobuf::Closure *done = google::protobuf::NewCallback<RpcProvider, const muduo::net::TcpConnectionPtr &, google::protobuf::Message *>(this, &RpcProvider::send_rpc_response, conn, response);

    //在框架上根据远端rpc请求，调用响应方法
    service->CallMethod(method, nullptr, request, response, done);
}

//Clouser的回调操作，用于序列化rpc的响应和网络发送，即序列化response
void RpcProvider::send_rpc_response(const TcpConnectionPtr &conn, google::protobuf::Message *response)
{
    string response_str;
    //序列化
    if (response->SerializeToString(&response_str))
    {
        //发送序列化的数据
        conn->send(response_str);
    }
    else
    {
        //序列化失败
        RPC_LOG_ERROR("serialize reponse error");
    }
    //短链接
    conn->shutdown();
}