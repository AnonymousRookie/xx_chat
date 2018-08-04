// Copyright 2018, AnonymousRookie.  All rights reserved.
// https://github.com/AnonymousRookie/xx_chat
// Author: AnonymousRookie (357688981 at qq dot com)


#include <QtWidgets/QApplication>
#include <QTextCodec>
#include "xx_chat_client.h"
#include "global.h"



int main(int argc, char *argv[])
{
    QTextCodec *codec = QTextCodec::codecForName("utf8");
    QTextCodec::setCodecForLocale(codec);
    
    QCoreApplication::setOrganizationName("xx");
    QCoreApplication::setApplicationName("xx_chat_client");

    QApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(false);


    XXChatClient::instance()->start();

    return a.exec();
}
