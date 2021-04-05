#pragma once
#include <unordered_map>
#include <string>
using namespace std;

/*
* 框架读取配置文件类
* rpcserver:ip rpcserver:port
* zookeeper:ip zookeeper:port
*/
class RpcConfigure
{
public:
    //加载配置文件
    void load_configure(const char *config_file);

    //查询配置项信息
    string find_load(string key);

    //去掉字符串前后的空格
    void trim(string &str_buf);

private:
    unordered_map<string, string> configure_map_;
};