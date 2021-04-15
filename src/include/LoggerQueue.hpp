#pragma once

#include <queue>
#include <mutex>
#include <thread>
#include <condition_variable>

using namespace std;

//异步写日志的日志队列
template <class T>
class LoggerQueue
{
public:
    void push(const T &data);
    T &pop();

private:
    queue<T> queue_;
    mutex mutex_;
    condition_variable condition_;
};