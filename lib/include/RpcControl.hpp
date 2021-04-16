#pragma once
#include <google/protobuf/service.h>
#include <string>

using namespace std;

class RpcControl : public google::protobuf::RpcController
{
public:
    RpcControl();

    void Reset();

    bool Failed() const;

    string ErrorText() const;

    void SetFailed(const string &reason);

public:
    //暂时不实现
    void StartCancel();

    bool IsCanceled() const;

    void NotifyOnCancel(google::protobuf::Closure *callback);

private:
    bool failed_;      //Rpc方法执行过程中的状态
    string error_text; //Rpc方法执行过程中的错误信息
};