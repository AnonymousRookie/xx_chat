// Copyright 2018, AnonymousRookie.  All rights reserved.
// https://github.com/AnonymousRookie/xx_chat
// Author: AnonymousRookie (357688981 at qq dot com)


#include "global.h"

Global* Global::instance_ = NULL;

Global::Global()
{
    QFile file(dataDir() + "/res.xml");
    file.open(QIODevice::ReadOnly);

    QDomDocument global_xml("xml");
    global_xml.setContent(&file);

    file.close();

    appicon_path_ = dataDir() + "/" + global_xml.documentElement().attribute("app_icon");
}