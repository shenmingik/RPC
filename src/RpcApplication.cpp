#include "RpcApplication.hpp"

#include <muduo/base/Logging.h>
#include <iostream>
#include <unistd.h>
#include <string>

using namespace muduo;
using namespace std;

RpcConfigure RpcApplication::configure_;

void RpcApplication::init(int argc, char **argv)
{
    if (argc < 2)
    {
        LOG_FATAL << "format: command -i <configfile>";
    }

    int choose = 0;
    string config_file;
    while ((choose = getopt(argc, argv, "i:")) != -1)
    {
        switch (choose)
        {
        case 'i':
            config_file = optarg;
            break;
        case '?':
            LOG_FATAL << " format: command -i <configfile>";
            break;
        case ':':
            LOG_FATAL << " format: command -i <configfile>";
            break;
        default:
            break;
        }
    }
    //加载配置文件
    configure_.load_configure(config_file.c_str());
}

RpcApplication::RpcApplication()
{
}
