#include "test.pb.h"
#include <iostream>

using namespace std;
using namespace ik;

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

    return 0;
}
