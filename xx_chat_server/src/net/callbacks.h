#ifndef Z_NET_SRC_CALLBACKS_H
#define Z_NET_SRC_CALLBACKS_H

#include <functional>
#include <memory>
#include "time_stamp.h"

NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(net)

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;

class Buffer;
class TcpConnection;

typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
typedef std::function<void()> TimerCallback;
typedef std::function<void(const TcpConnectionPtr&)> ConnectionCallback;
typedef std::function<void(const TcpConnectionPtr&)> CloseCallback;
typedef std::function<void(const TcpConnectionPtr&)> WriteCompleteCallback;
typedef std::function<void(const TcpConnectionPtr&, size_t)> HighWaterMarkCallback;
typedef std::function<void(const TcpConnectionPtr&, Buffer*, Timestamp)> MessageCallback;

void defaultConnectionCallback(const TcpConnectionPtr& conn);
void defaultMessageCallback(const TcpConnectionPtr& conn, Buffer* buffer, Timestamp receiveTime);

NAMESPACE_END(net)
NAMESPACE_END(z)

#endif  // Z_NET_SRC_CALLBACKS_H