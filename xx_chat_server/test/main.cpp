/**
 * Copyright 2018-2019, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: Client模拟程序
 */

#include "stdafx.h"
#include <stdio.h>
#include "cmd_thread.h"
#include "net_lib.h"

#ifdef _WIN32
    #pragma comment(lib, "ws2_32.lib")
#endif

int main(int argc, char** argv)
{
    Logger::GetInstance().SetFileBaseName("test_client");
    Logger::GetInstance().SetRollSize(10 * 1024 * 1024);
    Logger::GetInstance().Start();

    CmdThread cmdThread;
    cmdThread.Start();

    z::net::netlib_eventloop();

    return 0;
}
