#include "RpcProvider.hpp"

//框架提供给外部使用的，可以发布rpc方法的函数接口
void RpcProvider::notify_service(google::protobuf::Service *service)
{
}

//启动rpc服务节点，开始提供rpc远程网络调用服务
void RpcProvider::run()
{
}