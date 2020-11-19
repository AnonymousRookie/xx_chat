/**
 * Copyright 2019-2020, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: NONE
 */

#include <QtWidgets/QApplication>
#include <QFile>
#include <QTextStream>
#include <QTranslator>
#include <QSharedMemory>
#include <QMessageBox>
#include <QTimer>
#include <QDir>
#include "util\logger.h"
#include "util\file_util.h"
#include "util\string_util.h"
#include "network\net_lib.h"
#include "core\operation_manager.h"
#include "xx_chat.h"

#ifdef _WIN32
#pragma comment(lib, "base.lib")
#pragma comment(lib, "protobuf-lite.lib")
#pragma comment(lib, "curl.lib")
#pragma comment(lib, "ws2_32.lib")
#endif

 // 加载资源
void loadResource()
{
    // load stylesheet
    QFile file(":/xx_chat/qss");
    file.open(QFile::ReadOnly);
    QTextStream filetext(&file);
    QString stylesheet = filetext.readAll();
    qApp->setStyleSheet(stylesheet);
    file.close();
    // load icon
    // qApp->setWindowIcon(QIcon(":/xx_chat/icon"));
}

void initLogger(char* argv)
{
    QFileInfo fileinfo(z::utils::str2qstr(argv));
    QString logPath = fileinfo.absolutePath() + "/log";
    z::utils::createDirIfNotExist(logPath);

    std::ostringstream oss("");
    oss << z::utils::qStr2Str(logPath) << "/xx_chat_client";
    Logger::GetInstance().SetFileBaseName(oss.str().c_str());
    Logger::GetInstance().SetRollSize(10 * 1024 * 1024);  // 10M
    Logger::GetInstance().Start();
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    initLogger(argv[0]);
    loadResource();

    z::net::netlib_start_event();
    z::core::GetOperationManager()->Startup();
    XXChat::GetInstance()->Start();

    return a.exec();
}
