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
    void push(const T &data)
    {
        lock_guard<mutex> lock(mutex_);
        queue_.push(data);
        condition_.notify_one();
    }

    T pop()
    {
        unique_lock<mutex> lock(mutex_);
        while (queue_.empty())
        {
            //日志队列为空，线程进入wait
            condition_.wait(lock);
        }
        T ret = queue_.front();
        queue_.pop();
        return ret;
    }

private:
    queue<T> queue_;
    mutex mutex_;
    condition_variable condition_;
};