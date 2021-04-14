#include "RpcControl.hpp"

RpcControl::RpcControl()
{
    failed_ = false;
    error_text = "";
}

void RpcControl::Reset()
{
    failed_ = false;
    error_text = "";
}

bool RpcControl::Failed() const
{
    return failed_;
}

string RpcControl::ErrorText() const
{
    return error_text;
}

void RpcControl::SetFailed(const string &reason)
{
    failed_ = true;
    error_text = reason;
}

//暂时不实现
void RpcControl::StartCancel() {}

bool RpcControl::IsCanceled() const { return false; }

void RpcControl::NotifyOnCancel(google::protobuf::Closure *callback) {}