/**
 * Copyright 2019-2020, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: NONE
 */

#pragma once

#include <memory>
#include <QObject>
#include "base\src\defines.h"

NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(core)

class Event;

class EventManager : public QObject
{
    Q_OBJECT

signals:
    void TriggerEvent(Event* event);

private slots:
    void OnEventTrigger(Event* event);

public:
    EventManager();
    ~EventManager();

public:
    void AsyncFireEvent(Event* event);
};

EventManager* GetEventManager();

NAMESPACE_END(core)
NAMESPACE_END(z)
