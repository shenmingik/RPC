# RPC
RPC远程调用框架

# 使用示例
直接运行 autobuild.sh

邮箱：2107810343@qq.com

时间：2021/4/18 22:55

开发环境：Ubuntu VS Code

编译器：g++

编程语言：C++

# 认识RPC
RPC的全称是**远程过程调用**（Remote Procedure Call）。

什么是远程过程调用呢？

那么对于一个聊天系统有`int send_information(int friend_id,string msg)`这个方法，我们的一个处理逻辑是不是这样：
* 调用`bool is_exist(int friend_id)`判断用户是否在线
* 根据结果在决议是发送在线消息还是离线消息。

那么对于一个继承了登录和聊天功能的系统，我们在本地调用一个函数，就直接`返回值=函数名（参数1，参数2）`，就直接执行了这个过程并且得到了返回值。

但是如果考虑**高并发、高可用以及系统扩展性**的话，那我们不得不引入分布式的设计。这意味这，登录和聊天会部署在不同的机器上！那么要完成上面的逻辑，就不得不依靠网络，将要调用的函数名以及参数通过网络打包发送给另一个机器，然后由另一台机器将结果发过来。

而我们要做的RPC框架就是为使这个过程更好用而设计的。
![在这里插入图片描述](https://img-blog.csdnimg.cn/20210418224832597.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3NoZW5taW5neHVlSVQ=,size_16,color_FFFFFF,t_70)

# RPC框架的设计
博文的标题是基于 protobuf 和 zookeeper 的RPC框架，那么protobuf 和 zookeeper又在整个框架啊中扮演怎样的角色呢？

## protobuf 作用
protobuf 主要是作为整个框架的传输协议。我们可以看一下整个框架对于传输信息的格式定义：

```cpp
message RpcHeader
{
    bytes service_name = 1; 	//类名
    bytes method_name = 2;		//方法名
    uint32 args_size = 3;		//参数大小
}
```
可以看到，它**定义了要调用方法是属于哪个类的哪个方法以及这个方法所需要的的参数大小**。

那么参数呢？是怎样定义的？

首先我们来看一下我们框架内传输的数据是什么：
**4字节标识头部长度+RpcHeader+args**
```cpp
/*
* 16表示整个类名+方法名+参数长度的大小
	有个这个长度，我们就可以从整个长度中截取UserServiceLogin15这一段
	再根据RpcHeader来反序列话得出类名和方法名以及参数长度三个重要数据
* 15表示后面的参数长度
	由于我们找到了类名和方法名，我们就可以在整个框架存储这些信息的地方得到一个对于这个方法的描述。
	然后借用protobuf的service对象提供的一个接口GetResponsePrototype，并且根据这个方法的描述来反序列化出参数。这个都是根据我们注册的方法的描述来做的。
*/
18UserServiceLogin15zhang san123456
```

> **注：**
> 如果还是有点迷糊，可以保留参数解释信息，看到后面就大致懂了

## zookeeper
zookeeper 呢在这里其实主要就是起到了一个**配置中心**的目的。

什么意思呢？

zookeeper上面我们标识了每个类的方法所对应的**分布式节点地址**，当我们其他服务器想要RPC的时候，就去 zookeeper 上面查询对应要调用的服务在哪个节点上。

![在这里插入图片描述](https://img-blog.csdnimg.cn/20210417231712876.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3NoZW5taW5neHVlSVQ=,size_16,color_FFFFFF,t_70)

> **注意：**
> 这里就相当于，我其他服务器来 zookeeper 查询`User::is_exist` ，然后会得到127.0.0.1：8001 这个值，这个值就是你布置这个功能的一个RPC节点的网络标识符，然后向这个节点去发送参数并且等待这个节点的相应。

## 从框架的使用来认识
框架的使用一般都是在example目录下的 `callee/UserService.cpp` 里面

```cpp
    RpcApplication::init(argc, argv);

    //框架服务提供provider
    RpcProvider provide;
    provide.notify_service(new UserService());
    provide.run();
```
可以看到，主要去做了三个事情：
* 首先 RPC 框架肯定是部署到一台服务器上的，所以我们需要对这个服务器的 ip 和 port 进行初始化
* 然后创建一个 porvider（也就是server）对象，将当前 UserService 这个对象传递给他，也就是其实这个 RPC 框架和我们执行具体业务的节点是在同一个服务器上的。RPC框架负责解析其他服务器传递过来的请求，然后将这些参数传递给本地的方法。并将返回值返回给其他服务器。
* 最后是去让这个 provider 去 run 起来。具体我们其实可以看一下源代码：

```cpp
//启动rpc服务节点，开始提供rpc远程网络调用服务
void RpcProvider::run()
{
    //获取ip和port
    string ip = RpcApplication::get_instance().get_configure().find_load("rpcserver_ip");
    uint16_t port = atoi(RpcApplication::get_instance().get_configure().find_load("rpcserver_port").c_str());
    //cout << ip << ":" << port << endl;
    InetAddress address(ip, port);

    //创建tcpserver对象
    TcpServer server(&eventloop_, address, "RpcProvider");
    //绑定链接回调和消息读写回调方法
    server.setConnectionCallback(bind(&RpcProvider::on_connection, this, _1));
    server.setMessageCallback(bind(&RpcProvider::on_message, this, _1, _2, _3));

    //设置muduo库的线程数量
    server.setThreadNum(4);

    //把当前rpc节点上要发布的服务全部注册到zk上面，让rpc client可以从zk上发现服务
    ZookeeperClient zk_client;
    zk_client.start();

    //在配置中心中创建节点
    for (auto &sp : service_map_)
    {
        string service_path = "/" + sp.first;
        zk_client.create(service_path.c_str(), nullptr, 0);
        for (auto &mp : sp.second.method_map_)
        {
            string method_path = service_path + "/" + mp.first;
            char method_path_data[128] = {0};
            sprintf(method_path_data, "%s:%d", ip.c_str(), port);
            //ZOO_EPHEMERAL 表示znode时候临时性节点
            zk_client.create(method_path.c_str(), method_path_data, strlen(method_path_data), ZOO_EPHEMERAL);
        }
    }

    RPC_LOG_INFO("server RpcProvider [ip: %s][port: %d]", ip.c_str(), port);
    //启动网络服务
    server.start();
    eventloop_.loop();
}
```
可以看到，整个 run 其实就是干了这么几件事情：
* 因为底层调用的是muduo网络库，所以这里会获取ip地址和端口号，然后初始化网络层
* 然后去设置一个连接回调以及发生读写事件时候的回调函数（稍后介绍）
*  然后设置整个 muduo 网络库工作的线程数量
* 然后创建zookeeper配置中心，将这些方法的信息以及本机的IP地址注册到zookeeper
* 然后开启本机服务器的事件循环，等待其他服务器的连接

### 其他服务器是怎样调用的呢
我们看一下 example 目录下的 caller/CallUserService.cpp 里面是怎样调用的。

```cpp
    //初始化ip地址和端口号
    RpcApplication::init(argc, argv);

    //演示调用远程发布的rpc方法login
    ik::UserServiceRpc_Stub stub(new RpcChannel());

    //请求参数和响应
    ik::LoginRequest request;
    request.set_name("zhang san");
    request.set_password("123456");

    ik::LoginResponse response;
    RpcControl control;
    //发起rpc调用，等待响应返回
    stub.Login(&control, &request, &response, nullptr);

    //rpc调用完成，读调用的结果
    if (response.errmsg().error() == 0)
    {
        //没错误
        cout << "rpc login response: " << response.success() << endl;
    }
    else
    {
        //有错误
        cout << "rpc login response errer: " << response.errmsg().error_msg() << endl;
    }
```
同样，也是有以下几个步骤：
* 初始化 RPC 远程调用要连接的服务器
* 定义一个 UserSeervice 的 stub 桩类，由这个装类去调用Login方法，这个login方法可以去看一下源码的定义：
	

```cpp
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
```
可以看到，Login的 RPC 重载函数有四个参数：**controller（表示函数是否出错）、request（参数）、response（返回值）、done（回调函数）**
其主要做的也是去围绕着解析参数，将参数放入本地调用的方法，将结果返回并执行回调函数。至于这个回调函数则是在服务端执行读写事件回调函数绑定的。

绑定的是如下方法：

```cpp
void RpcProvider::send_rpc_response(const TcpConnectionPtr &conn, google::protobuf::Message *response)
{
    string response_str;
    //序列化
    if (response->SerializeToString(&response_str))
    {
        //发送序列化的数据
        conn->send(response_str);
    }
    else
    {
        //序列化失败
        RPC_LOG_ERROR("serialize reponse error");
    }
    //短链接
    conn->shutdown();
}
```
它会将由`bind`函数绑定的参数：`response_str` 发送回去，然后调用方的服务器就会收到这个返回值并解析它。

#### 桩类是干嘛的
那么其实现在来说，我们并没有看到调用方是何时发送了要调用方法以及相应参数？

我们还是需要去返回桩类，这个是由 **protobuf** 自动去帮你生成的。

```cpp
class UserServiceRpc_Stub : public UserServiceRpc {
 public:
  UserServiceRpc_Stub(::google::protobuf::RpcChannel* channel);
  UserServiceRpc_Stub(::google::protobuf::RpcChannel* channel,
                   ::google::protobuf::Service::ChannelOwnership ownership);
  ~UserServiceRpc_Stub();

  inline ::google::protobuf::RpcChannel* channel() { return channel_; }

  // implements UserServiceRpc ------------------------------------------

  void Login(::google::protobuf::RpcController* controller,
                       const ::ik::LoginRequest* request,
                       ::ik::LoginResponse* response,
                       ::google::protobuf::Closure* done);
  void Register(::google::protobuf::RpcController* controller,
                       const ::ik::RegisterRequest* request,
                       ::ik::RegisterResponse* response,
                       ::google::protobuf::Closure* done);
 private:
  ::google::protobuf::RpcChannel* channel_;
  bool owns_channel_;
  GOOGLE_DISALLOW_EVIL_CONSTRUCTORS(UserServiceRpc_Stub);
};
```
我们在定义桩类的时候，会传入一个RpcCannel的指针，这个绑定到这个桩类的channel_指针。
当我们去调用这个桩类的`Login`方法的时候，会去调用传递进来的channel的`CallMethod`方法：

```cpp
void UserServiceRpc_Stub::Login(::google::protobuf::RpcController* controller,
                              const ::ik::LoginRequest* request,
                              ::ik::LoginResponse* response,
                              ::google::protobuf::Closure* done) {
  channel_->CallMethod(descriptor()->method(0),
                       controller, request, response, done);
}
```
所以，显而易见，我去发送这些方法啊，参数啊，都是在`CallMethod`这个方法中执行的。

那么`CallMethod`里面执行的内容对我们理解RPC调用体系至关重要（代码比较长，可以直接跳过听结论）：

```cpp
void RpcChannel::CallMethod(const google::protobuf::MethodDescriptor *method,
                            google::protobuf::RpcController *controller,
                            const google::protobuf::Message *request,
                            google::protobuf::Message *response,
                            google::protobuf::Closure *done)
{
    const google::protobuf::ServiceDescriptor *service_des = method->service();
    string service_name = service_des->name();
    string method_name = method->name();

    //获取参数的序列化字符串长度 args_size
    int args_size = 0;
    string args_str;
    if (request->SerializeToString(&args_str))
    {
        args_size = args_str.size();
    }
    else
    {
        controller->SetFailed("serialize request error!");
        return;
    }

    //定义rpc的请求header
    ikrpc::RpcHeader rpc_header;
    rpc_header.set_service_name(service_name);
    rpc_header.set_method_name(method_name);
    rpc_header.set_args_size(args_size);

    uint32_t header_size = 0;
    string rpc_header_str;
    if (rpc_header.SerializeToString(&rpc_header_str))
    {
        //序列化成功
        header_size = rpc_header_str.size();
    }
    else
    {
        //序列化失败
        controller->SetFailed("serialize rpc_header error!");
        return;
    }

    //组织待发送的rpc请求的字符串
    string send_rpc_str;
    send_rpc_str.insert(0, string((char *)&header_size, 4)); //header_size
    send_rpc_str += rpc_header_str;                          //rpc_header
    send_rpc_str += args_str;                                //args_str

    //使用tcp编程，完成rpc方法的远程调用
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd == -1)
    {
        close(client_fd);
        RPC_LOG_FATAL("create socket error! errno:%d", errno);
    }

    //获取ip和port
    ZookeeperClient zk_client;
    zk_client.start();
    string method_path = "/" + service_name + "/" + method_name;
    string host_data = zk_client.get_data(method_path.c_str());
    if(host_data == "")
    {
        controller->SetFailed(method_path+" is not exist");
        return;
    }
    int host_index = host_data.find(":");
    if(host_index == -1)
    {
        controller->SetFailed(method_path+" address is invalid!");
        return;
    }
    string ip = host_data.substr(0,host_index);
    uint16_t port = atoi(host_data.substr(host_index+1,host_data.size()-host_index).c_str());

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip.c_str());

    if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        close(client_fd);
        RPC_LOG_FATAL("connet error! errno: %d", errno);
    }

    // 发送rpc请求
    if (send(client_fd, send_rpc_str.c_str(), send_rpc_str.size(), 0) == -1)
    {
        controller->SetFailed("send error! errno: " + errno);
        close(client_fd);
        return;
    }

    //接受rpc请求
    char recv_buffer[BUFF_SIZE] = {0};
    ssize_t recv_size = recv(client_fd, recv_buffer, BUFF_SIZE, 0);
    if (recv_size == -1)
    {
        controller->SetFailed("recv error! errno: " + errno);
        close(client_fd);
        return;
    }

    //反序列化响应数据
    //String 因为遇到\0会认为是字符串结束，所以用Array
    if (!response->ParseFromArray(recv_buffer, recv_size))
    {
        string recv = recv_buffer;
        controller->SetFailed("parse error! response_str: " + recv);
        close(client_fd);
        return;
    }
    close(client_fd);
}
```
* 组织要发送的 request_str 字符串
* 从zookeeper中拿到服务端的 ip 和 port，连接服务端
* 发送 request_str
* 接受服务端返回过来的 response 字符串并反序列化出结果

# 总结一下RPC流程
![在这里插入图片描述](https://img-blog.csdnimg.cn/20210418224541734.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3NoZW5taW5neHVlSVQ=,size_16,color_FFFFFF,t_70)
# 参考文献
	[1] 施磊.分布式网络通信RPC框架.腾讯课堂
