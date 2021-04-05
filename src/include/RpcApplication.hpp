#pragma once

//Rpc框架的基础类(单例)
class RpcApplication
{
public:
    static void init(int argc, char **argv);

    static RpcApplication &get_instance()
    {
        static RpcApplication instance;
        return instance;
    }

private:
    RpcApplication();
    ~RpcApplication();

private:
    RpcApplication(RpcApplication &) = delete;
    RpcApplication(RpcApplication &&) = delete;
};