// Copyright 2018, AnonymousRookie.  All rights reserved.
// https://github.com/AnonymousRookie/xx_chat
// Author: AnonymousRookie (357688981 at qq dot com)


#ifndef XX_CHAT_CLIENT_SRC_GLOBAL_H
#define XX_CHAT_CLIENT_SRC_GLOBAL_H

#include <QtXml/QDomDocument>
#include <QString>
#include <QDir>

class Global
{
public:
    static Global* instance() {
        if (!instance_) {
            instance_ = new Global();
        }
        return instance_;
    }

    static QString appName() {
        static const QString app_name_ = "xx_chat";
        return app_name_;
    }

    static QString dataDir() {
        static const QString data_path = "D:/SourceCodes/xx_chat_client/xx_chat_client/data";
        return data_path;
    }

    static QString configDir() {
        QString config_path = QDir::homePath() + "/.config/" + appName();
        QDir config_dir(config_path);
        if (!config_dir.exists()) {
            config_dir.mkdir(config_path);
        }
        return config_path;
    }

    static QString pluginDir() {
        static const QString plugin_dir = "";
        return plugin_dir;
    }

    static QString version() {
        static const QString xx_chat_client_version = "";
        return xx_chat_client_version;
    }

    static QString skinsDir() {
        return dataDir() + "/skins";
    }

    static QString resourceDir() {
        return dataDir() + "/res";
    }

    static QString tempDir() {
        QString temp_path = QDir::tempPath() + "/" + appName();
        QDir temp_dir(temp_path);
        if (!temp_dir.exists()) {
            temp_dir.mkdir(temp_path);
        }
        return temp_path;
    }

    QString appIconPath() const {
        return appicon_path_;
    }


private:
    QString appicon_path_;
    static const QString version_;

private:
    Global();
    Global(const Global&);
    Global& operator=(const Global&);

    static Global* instance_;
};




#endif  // XX_CHAT_CLIENT_SRC_GLOBAL_H