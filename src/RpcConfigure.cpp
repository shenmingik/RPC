#include "RpcConfigure.hpp"
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
        LOG_FATAL << config_file << " is not exist!";
    }

    // 1.注释 2.正确的配置项 3.去掉开头多余的空格
    while (!feof(pf))
    {
        char buf[BUFFER_SIZE] = {0};
        fgets(buf, BUFFER_SIZE, pf);

        //检查空格并去掉
        string str_buf(buf);
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

        //判断# 注释 或者空行
        if (str_buf[0] == '#' || str_buf.empty())
        {
            continue;
        }

        // 解析配置项
        index = str_buf.find('=');
        if (index == -1)
        {
            LOG_ERROR << "configure file illegal";
            continue;
        }

        string key = str_buf.substr(0, index);
        string value = str_buf.substr(index + 1, str_buf.size() - index);
        configure_map_.insert({key, value});
        cout << "key: " << key << "value: " << value << endl;
    }
}

//查询配置项信息
string RpcConfigure::find_load(string &key)
{
    auto it = configure_map_.find(key);
    if (it == configure_map_.end())
    {
        return "";
    }
    return it->second;
}