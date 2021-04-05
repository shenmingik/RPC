#include <iostream>
#include <string>
#include "User.pb.h"
#include "RpcApplication.hpp"

using namespace std;
/*
* UserService 是一个本地服务，提供了两个进程内的本地方法，login和getfriendlist
*/
class UserService : public ik::UserServiceRpc //使用在rpc服务发布端
{
public:
    bool Login(string name, string password)
    {
        cout << "doing local service:login: " << endl;
        cout << "name: " << name << " password: " << password << endl;
    }

    //重写基类UserServiceRpc的虚函数，供框架调用
    void Login(::google::protobuf::RpcController *controller,
               const ::ik::LoginRequest *request,
               ::ik::LoginResponse *response,
               ::google::protobuf::Closure *done)
    {
        //框架给业务上报了请求参数 request，业务获取相应数据做本地业务
        string name = request->name();
        string password = request->password();

        //本地业务
        bool login_result = Login(name, password);

        //把响应给调用方返回
        ik::ErrorMsg *errmsg = response->mutable_errmsg();
        errmsg->set_error(0);
        errmsg->set_error_msg("");
        response->set_success(login_result);

        //执行回调操作
        done->Run();
    }
};

int main(int argc,char** argv)
{
    //调用框架的初始化操作

    //框架服务提供provider

    return 0;
}