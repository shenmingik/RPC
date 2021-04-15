#include "RpcLogger.hpp"
#include <muduo/base/Logging.h>
#include <unistd.h>

#define FILENAME_SIZE 128
#define TIME_BUFFER 128

void RpcLogger::log_info(string msg)
{
    set_level(INFO);
    log(msg);
}
void RpcLogger::log_error(string msg)
{
    set_level(ERROR);
    log(msg);
}
void RpcLogger::log_fatal(string msg)
{
    set_level(FATAL);
    log(msg);
    LOG_ERROR << "Force Majeure is being dealt with";
    //sleep(10);
    LOG_ERROR << "Question has been record in log file, please restore system,bye";
    exit(0);
}

//设置日志级别
void RpcLogger::set_level(LogLevel level)
{
    log_level_ = level;
}

//写日志
void RpcLogger::log(string msg)
{
    string begin_info;
    switch (log_level_)
    {
    case INFO:
        begin_info = "[INFO]: ";
        break;
    case ERROR:
        begin_info = "[ERROR]: ";
        break;
    case FATAL:
        begin_info = "[FATAL]: ";
        break;
    default:
        break;
    }
    begin_info += msg;
    log_queue_.push(begin_info);
}

// 启动写日志线程，负责写日志
RpcLogger::RpcLogger()
{
    thread wirte_thread([&]() {
        for (;;)
        {
            //获取当前日期，取日志信息，写入相应的日志文件当中
            time_t now = time(nullptr);
            tm *now_time = localtime(&now);

            char file_name[FILENAME_SIZE] = {0};
            sprintf(file_name, "%d-%d-%d_log.txt", now_time->tm_year + 1900, now_time->tm_mon + 1, now_time->tm_mday);

            FILE *file_ptr = fopen(file_name, "a+");
            if (file_ptr == nullptr)
            {
                //打印muduo错误日志
                LOG_ERROR << "logger file: " << file_name << " opne error";
            }

            string msg;
            char time_buf[TIME_BUFFER] = {0};
            sprintf(time_buf, "%2d:%2d:%2d=>", now_time->tm_hour, now_time->tm_min, now_time->tm_sec);
            msg = log_queue_.pop();
            msg.insert(0, time_buf);
            msg += '\n';
            fputs(msg.c_str(), file_ptr);

            fclose(file_ptr);
        }
    });

    //设置分离线程
    wirte_thread.detach();
}