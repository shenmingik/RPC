#pragma once

#include "LoggerQueue.hpp"
#include <string>

using namespace std;

enum LogLevel
{
    INFO,  //普通信息
    ERROR, //错误信息
    FATAL, //毁灭信息
};

//日志系统
class RpcLogger
{
public:
    void log_info(string msg);
    void log_error(string msg);
    void log_fatal(string msg);

    RpcLogger &get_instance()
    {
        static RpcLogger instance;
        return instance;
    }

private:
    //设置日志级别
    void set_level(LogLevel level);

    //写日志
    void log(string msg);

private:
    RpcLogger();
    RpcLogger(const RpcLogger &) = delete;
    RpcLogger(RpcLogger &&) = delete;
    RpcLogger &operator=(const RpcLogger &) = delete;

private:
    int log_level_;
    LoggerQueue<string> log_queue_; //日志缓冲队列
};