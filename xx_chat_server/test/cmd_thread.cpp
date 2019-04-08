/**
 * Copyright 2018-2019, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: 处理控制台输入命令并执行线程
 */

#include "stdafx.h"
#include "cmd_thread.h"
#include "client.h"

CmdThread::CmdThread()
{
}

CmdThread::~CmdThread()
{
    Stop();
}

void CmdThread::Start()
{
    if (started_) {
        return;
    }
    started_ = true;
    thread_ = std::thread(&CmdThread::RunInThread, this);
}

void CmdThread::Stop()
{
    started_ = false;
    if (thread_.joinable()) {
        thread_.join();
    }
}

void CmdThread::RunInThread()
{
    while (started_) {
        if (NULL == fgets(cmdBuf_, MAX_LINE_SIZE - 1, stdin)) {
            fprintf(stderr, "fgets error!\n");
            continue;
        }
        cmdBuf_[strlen(cmdBuf_) - 1] = '\0';
        SplitCmd(cmdBuf_);
        ExecCmd();
    }
}

void CmdThread::SplitCmd(char* buf)
{
    cmds_.clear();
    std::string cmd("");
    for (int i = 0; i < strlen(buf); ++i) {
        if (buf[i] == ' ' || buf[i] == '\t') {
            if (!cmd.empty()) {
                cmds_.push_back(cmd);
                cmd.clear();
            }
        }
        else {
            cmd += buf[i];
        }
    }
    // put the last one
    if (!cmd.empty()) {
        cmds_.push_back(cmd);
        cmd.clear();
    }
}

void CmdThread::ExecCmd()
{
    if (cmds_.empty()) {
        return;
    }
    if (cmds_[0] == "login") {
        if (cmds_.size() == 3) {
            DoLogin("http://127.0.0.1:8080", cmds_[1], cmds_[2]);
        }
        else {
            PrintHelp();
        }
    }
    else if (cmds_[0] == "close") {

    }
    else if (cmds_[0] == "quit") {

    }
    else {
        PrintHelp();
    }
}

void CmdThread::PrintHelp()
{
    printf("Unknown cmd! Usage:\n");
    printf("login username password\n");
    printf("close\n");
    printf("quit\n");
}

void CmdThread::DoLogin(const std::string& domain, const std::string& username, const std::string& password)
{
    client_ = std::make_shared<Client>(username, password, domain);
    client_->Connect();
}
