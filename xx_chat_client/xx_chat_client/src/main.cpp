/**
 * Copyright 2019-2020, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: NONE
 */

#include <QtWidgets/QApplication>
#include "util\logger.h"
#include "network\net_lib.h"
#include "core\operation_manager.h"
#include "xx_chat.h"

#ifdef _WIN32
#pragma comment(lib, "base.lib")
#pragma comment(lib, "protobuf-lite.lib")
#pragma comment(lib, "curl.lib")
#pragma comment(lib, "ws2_32.lib")
#endif

void InitLogger()
{
    Logger::GetInstance().SetFileBaseName("xx_chat_client");
    Logger::GetInstance().SetRollSize(10 * 1024 * 1024);
    Logger::GetInstance().Start();
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    InitLogger();

    z::net::netlib_start_event();
    z::core::GetOperationManager()->Startup();
    XXChat::GetInstance()->Start();

    return a.exec();
}
