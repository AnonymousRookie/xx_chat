/**
 * Copyright 2019-2020, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: NONE
 */

#pragma once

#include <string>
#include <functional>
#include <memory>
#include "base\src\defines.h"

NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(core)

typedef std::function<void(std::shared_ptr<void>)> OperationDelegate;

// �첽����ִ�е�λ
struct IOperation
{
public:
    virtual void Process() = 0;
};


class Operation : public IOperation
{
    enum OperationState
    {
        OPERATION_IDLE = 0,
        OPERATION_STARTING,
        OPERATION_RUNNING,
        OPERATION_CANCELLING,
        OPERATION_FINISHED,
    };

public:
    Operation();
    Operation(const std::string& name);
    virtual ~Operation();

public:
    virtual void ProcessOperation() = 0;

public:
    virtual void Process();

    inline std::string GetName() const { return name_; }
    inline void SetName(const std::string& name) { name_ = name; }

private:
    OperationState state_ = OPERATION_IDLE;
    std::string name_ = "operation_name_common";
};


class CallbackOperation : public Operation
{
public:
    CallbackOperation(OperationDelegate& callback);
    virtual ~CallbackOperation();

    void SyncCallback(std::shared_ptr<void> param);

private:
    OperationDelegate callback_;
};


class IPduPacketParse
{
public:
    // �յ�TcpClient�����֮��Ļص��ӿ�
    virtual void OnPacket() = 0;
};


NAMESPACE_END(core)
NAMESPACE_END(z)
