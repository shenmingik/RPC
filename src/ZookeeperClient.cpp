#include "ZookeeperClient.hpp"
#include "RpcApplication.hpp"
#include "RpcLogger.hpp"
#include <iostream>

using namespace std;

void global_watcher(zhandle_t *handler, int type, int state, const char *path, void *wathcer_context)
{
    if (type == ZOO_SESSION_EVENT) //回调的消息类型
    {
        //连接成功就会发送一个成功信号
        if (state == ZOO_CONNECTED_STATE) //zkclient 和 zkserver 连接成功
        {
            //获取信号量
            sem_t *sem = (sem_t *)zoo_get_context(handler);
            sem_post(sem);
        }
    }
}

ZookeeperClient::ZookeeperClient() : zhandle_(nullptr)
{
}
ZookeeperClient::~ZookeeperClient()
{
    if (zhandle_ != nullptr)
    {
        zookeeper_close(zhandle_); //关闭句柄，释放资源
    }
}

//启动连接--》zkserver
// 因为是异步，所以需要同步
void ZookeeperClient::start()
{
    string host = RpcApplication::get_instance().get_configure().find_load("zookeeper_ip");
    string port = RpcApplication::get_instance().get_configure().find_load("zookeeper_port");
    string con_str = host + ":" + port;
    cout << con_str << endl;
    zhandle_ = zookeeper_init(con_str.c_str(), global_watcher, 30000, nullptr, nullptr, 0);
    if (zhandle_ == nullptr)
    {
        RPC_LOG_FATAL("zookeeper init error");
    }

    sem_t sem;
    sem_init(&sem, 0, 0);
    zoo_set_context(zhandle_, &sem); //设置信号量s

    sem_wait(&sem);
    RPC_LOG_INFO("zookeeper init success");
}

//在zkserver 根据指定的path创建znode节点
void ZookeeperClient::create(const char *path, const char *data, int datalen, int state)
{
    char path_buffer[128] = {0};
    int buffer_len = sizeof(path_buffer);
    int flag;

    //同步检查path是否存在
    flag = zoo_exists(zhandle_, path, 0, nullptr);
    if (ZNONODE == flag) //不存在
    {
        flag = zoo_create(zhandle_, path, data, datalen, &ZOO_OPEN_ACL_UNSAFE, state, path_buffer, buffer_len);
        if (flag == ZOK) //成功
        {
            RPC_LOG_INFO("znode create  success...path: %s", path);
        }
        else
        {
            RPC_LOG_FATAL("falg: %d, znode create error... path: %s", flag, path);
        }
    }
}
//根据参数指定的znode节点路径，获取znode节点的值
string ZookeeperClient::get_data(const char *path)
{
    //buffer存储返回结果
    char buffer[64] = {0};
    int buffer_len = sizeof(buffer);
    int flag = zoo_get(zhandle_, path, 0, buffer, &buffer_len, nullptr);
    if (flag != ZOK)
    {
        RPC_LOG_ERROR("can't get znode... path: %s", path);
        return "";
    }
    else
    {
        return buffer;
    }
}
