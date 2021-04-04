#include "test.pb.h"
#include <iostream>

using namespace std;
using namespace ik;

int main()
{
    LoginResponse login_rsp;
    ErrorMsg *error_msg = login_rsp.mutable_error();
    error_msg->set_error(1);
    error_msg->set_error_msg("login error");

    GetFriendListResponse friend_rsp;
    ErrorMsg *friend_msg = friend_rsp.mutable_error();
    friend_msg->set_error(1);
    friend_msg->set_error_msg("get friend list error");

    User *user1 = friend_rsp.add_friendlists();
    user1->set_name("zhang san");
    user1->set_age(20);
    user1->set_sex(User::man);

    cout << "has friends: " << friend_rsp.friendlists_size() << endl;
}

#if 0
int main()
{
    LoginRequest req;
    req.set_name("zhangsan");
    req.set_password("123456");

    //序列化
    string str_req;
    if(req.SerializeToString(&str_req))
    {
        cout << "after serialize" << str_req << endl;
    }

    //反序列化
    LoginRequest new_req;
    if(!new_req.ParseFromString(str_req))
    {
        cout<<"unserialize error!"<<endl;
    }
    else
    {
        cout<<"unserialize success"<<endl;
        cout<<"name: "<<new_req.name()<<" password: "<<new_req.password()<<endl;
    }

    return 0;
}
#endif