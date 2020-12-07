#ifndef Z_NET_SRC_TCP_CLIENT_H
#define Z_NET_SRC_TCP_CLIENT_H

#include <string>
#include <mutex>
#include <assert.h>
#include "defines.h"
#include "tcp_connection.h"

NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(net)

class Connector;
class EventLoop;
typedef std::shared_ptr<Connector> ConnectorPtr;

class TcpClient
{
public:
    TcpClient(EventLoop* loop, const NetAddress& serverAddr, const std::string& name);
    ~TcpClient();

    void connect();
    void disconnect();
    void stop();

    TcpConnectionPtr connection() const
    {
        std::unique_lock<std::mutex> lock(mutex_);
        return connection_;
    }

    EventLoop* getLoop() const { return loop_; }

    const std::string& name() const
    {
        return name_;
    }

    void setConnectionCallback(const ConnectionCallback& cb)
    {
        connectionCallback_ = cb;
    }

    void setMessageCallback(const MessageCallback& cb)
    {
        messageCallback_ = cb;
    }

    void setWriteCompleteCallback(const WriteCompleteCallback& cb)
    {
        writeCompleteCallback_ = cb;
    }

private:
    void newConnection(int sockfd);
    void removeConnection(const TcpConnectionPtr& conn);

private:
    EventLoop* loop_;
    ConnectorPtr connector_;
    std::string name_;
    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    WriteCompleteCallback writeCompleteCallback_;
    bool connect_;

    int nextConnId_;
    mutable std::mutex mutex_;
    TcpConnectionPtr connection_;
};

NAMESPACE_END(net)
NAMESPACE_END(z)

#endif  // Z_NET_SRC_TCP_CLIENT_H