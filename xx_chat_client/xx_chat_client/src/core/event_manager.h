/**
 * Copyright 2019-2020, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: NONE
 */

#pragma once

#include <list>
#include <memory>
#include <QObject>
#include <QTimer>
#include "base\src\defines.h"

NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(core)

class Event;
class TimerEvent;

class EventManager : public QObject
{
    Q_OBJECT

signals:
    void TriggerEvent(Event* event);

private slots:
    void OnEventTrigger(Event* event);
    void OnTimeout();

public:
    EventManager();
    ~EventManager();

public:
    void AsyncFireEvent(Event* event);
    void Startup();
    void Shutdown();

    void RegisterTimerEvent(std::shared_ptr<TimerEvent> event);
    void UnRegisterTimerEvent(std::shared_ptr<TimerEvent> event);

private:
    std::shared_ptr<QTimer> spTimer_ = nullptr;
    std::list<std::shared_ptr<TimerEvent>> lstTimerEvent_;
};

EventManager* GetEventManager();

NAMESPACE_END(core)
NAMESPACE_END(z)
