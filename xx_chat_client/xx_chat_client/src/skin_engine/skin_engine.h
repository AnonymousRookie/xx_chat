// Copyright 2018, AnonymousRookie.  All rights reserved.
// https://github.com/AnonymousRookie/xx_chat
// Author: AnonymousRookie (357688981 at qq dot com)


#ifndef XX_CHAT_CLIENT_SRC_SKINENGINE_SKINENGINE_H
#define XX_CHAT_CLIENT_SRC_SKINENGINE_SKINENGINE_H

#include <QtXml/QDomDocument>
#include <QApplication>
#include <QFile>

#include "global.h"


class SkinEngine : public QObject
{
    Q_OBJECT

signals:
    void skinChanged();

public:
    static SkinEngine* instance() 
    {
        if (!instance_) {
            instance_ = new SkinEngine();
        }
        return instance_;
    }

    void setSkin(const QString& skin_name)
    {
        cur_skin_ = skin_name;

        QFile file(curSkinDir() + "/style.css");
        file.open(QIODevice::ReadOnly);
        QString style = file.readAll();
        style.replace("%skin_path%", curSkinDir());
        qApp->setStyleSheet(style);

        emit skinChanged();
    }

    QString skinRes(const QString& res_name)
    {
        return curSkinDir() + "/" + res_.documentElement().attribute(res_name, "unset");
    }

    QString curSkinDir() const
    {
        return Global::skinsDir() + "/" + cur_skin_;
    }

private:
    SkinEngine(QObject* parent = NULL) : QObject(parent),
        res_("xml"),
        cur_skin_("default")
    {
        QFile file(curSkinDir() + "/res.xml");
        file.open(QIODevice::ReadOnly);

        res_.setContent(&file);
        file.close();

        setSkin(cur_skin_);
    }

    SkinEngine(const SkinEngine&);
    SkinEngine& operator=(const SkinEngine&);

private:
    QDomDocument res_;
    QString cur_skin_;

private:
    static SkinEngine* instance_;
};





#endif  // XX_CHAT_CLIENT_SRC_SKINENGINE_SKINENGINE_H