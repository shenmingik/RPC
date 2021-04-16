#include "RpcConfigure.hpp"
#include "RpcLogger.hpp"
#include <muduo/base/Logging.h>
#include <string>
#include <iostream>
using namespace std;

#define BUFFER_SIZE 128

//加载配置文件
void RpcConfigure::load_configure(const char *config_file)
{
    FILE *pf = fopen(config_file, "r");
    if (pf == nullptr)
    {
        RPC_LOG_FATAL("%s is not exist!", config_file);
    }

    // 1.注释 2.正确的配置项 3.去掉开头多余的空格
    while (!feof(pf))
    {
        char buf[BUFFER_SIZE] = {0};
        fgets(buf, BUFFER_SIZE, pf);

        string str_buf(buf);

        trim(str_buf);

        //判断# 注释 或者空行
        if (str_buf[0] == '#' || str_buf[0] == '\n' || str_buf.empty())
        {
            continue;
        }

        // 解析配置项
        int index = str_buf.find('=');
        if (index == -1)
        {
            RPC_LOG_ERROR("configure file illegal!");
            continue;
        }

        string key = str_buf.substr(0, index);
        trim(key);

        string value = str_buf.substr(index + 1, str_buf.size() - index);
        //去除最后一个换行符'\n'
        value[value.size() - 1] = ' ';
        trim(value);

        configure_map_.insert({key, value});
    }
}

//查询配置项信息
string RpcConfigure::find_load(string key)
{
    auto it = configure_map_.find(key);
    if (it == configure_map_.end())
    {
        return "";
    }
    return it->second;
}

//去掉字符串前后的空格
void RpcConfigure::trim(string &str_buf)
{
    //检查空格并去掉
    //找到第一个不为空格的字符
    int index = str_buf.find_first_not_of(' ');
    if (index != -1)
    {
        //说明前面有字符，截取字符
        str_buf = str_buf.substr(index, str_buf.size() - index);
    }
    //去掉字符串后面的空格，找到最后一个不为空格的字符
    index = str_buf.find_last_not_of(' ');
    if (index != -1)
    {
        //说明后面有空格，截取字符
        str_buf = str_buf.substr(0, index + 1);
    }
}