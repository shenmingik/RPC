#include "RpcChannel.hpp"
#include "RpcApplication.hpp"
#include "RpcHeader.pb.h"
#include <muduo/base/Logging.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <errno.h>

using namespace std;

#define BUFF_SIZE 1024

void RpcChannel::CallMethod(const google::protobuf::MethodDescriptor *method,
                            google::protobuf::RpcController *controller,
                            const google::protobuf::Message *request,
                            google::protobuf::Message *response,
                            google::protobuf::Closure *done)
{
    const google::protobuf::ServiceDescriptor *service_des = method->service();
    string service_name = service_des->name();
    string method_name = method->name();

    //获取参数的序列化字符串长度 args_size
    int args_size = 0;
    string args_str;
    if (request->SerializeToString(&args_str))
    {
        args_size = args_str.size();
    }
    else
    {
        LOG_ERROR << "serialize request error!";
        return;
    }

    //定义rpc的请求header
    ikrpc::RpcHeader rpc_header;
    rpc_header.set_service_name(service_name);
    rpc_header.set_method_name(method_name);
    rpc_header.set_args_size(args_size);

    uint32_t header_size = 0;
    string rpc_header_str;
    if (rpc_header.SerializeToString(&rpc_header_str))
    {
        //序列化成功
        header_size = rpc_header_str.size();
    }
    else
    {
        //序列化失败
        LOG_ERROR << "serialize rpc_header error!";
        return;
    }

    //组织待发送的rpc请求的字符串
    string send_rpc_str;
    send_rpc_str.insert(0, string((char *)&header_size, 4)); //header_size
    send_rpc_str += rpc_header_str;                          //rpc_header
    send_rpc_str += args_str;                                //args_str

    //打印调试信息
    cout << "<------------------------------------------------>" << endl;
    cout << "rpc header str: " << rpc_header_str << endl;
    cout << "header size: " << header_size << endl;
    cout << "service name: " << service_name << endl;
    cout << "method name: " << method_name << endl;
    cout << "args: " << args_str << endl;
    cout << "send rpc str: " << send_rpc_str << endl;
    cout << "<------------------------------------------------>" << endl;

    //使用tcp编程，完成rpc方法的远程调用
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd == -1)
    {
        close(client_fd);
        LOG_FATAL << "create socket error! errno: " << errno;
    }

    //获取ip和port
    string ip = RpcApplication::get_instance().get_configure().find_load("rpcserver_ip");
    uint16_t port = atoi(RpcApplication::get_instance().get_configure().find_load("rpcserver_port").c_str());

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip.c_str());

    if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        close(client_fd);
        LOG_FATAL << "connet error! errno: " << errno;
    }

    // 发送rpc请求
    if (send(client_fd, send_rpc_str.c_str(), send_rpc_str.size(), 0) == -1)
    {
        LOG_ERROR << "send error! errno: " << errno;
        close(client_fd);
        return;
    }

    //接受rpc请求
    char recv_buffer[BUFF_SIZE] = {0};
    ssize_t recv_size = recv(client_fd, recv_buffer, BUFF_SIZE, 0);
    if (recv_size == -1)
    {
        LOG_ERROR << "recv error! errno: " << errno;
        close(client_fd);
        return;
    }

    //反序列化响应数据
    //String 因为遇到\0会认为是字符串结束，所以用Array
    if (!response->ParseFromArray(recv_buffer, recv_size))
    {
        LOG_ERROR << "parse error! response_str: " << recv_buffer;
        close(client_fd);
        return;
    }
    close(client_fd);
}