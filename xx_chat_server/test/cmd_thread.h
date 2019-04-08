/**
 * Copyright 2018-2019, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: 处理控制台输入命令并执行线程
 */

#ifndef TEST_CLIENT_CMD_THREAD_H
#define TEST_CLIENT_CMD_THREAD_H

#include <memory>
#include <thread>
#include <string>
#include <vector>

class Client;
class CmdThread
{
public:
    CmdThread();
    ~CmdThread();

public:
    void Start();
    void Stop();

private:
    void RunInThread();

    void SplitCmd(char* buf);
    void ExecCmd();
    void PrintHelp();
    void DoLogin(const std::string& domain, 
                 const std::string& username, 
                 const std::string& password);

private:
    bool started_{ false };
    std::thread thread_;
    enum { MAX_LINE_SIZE = 1024 };
    char cmdBuf_[MAX_LINE_SIZE];
    std::vector<std::string> cmds_;
    std::shared_ptr<Client> client_;
};

#endif  // TEST_CLIENT_CMD_THREAD_H