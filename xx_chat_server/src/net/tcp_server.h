#ifndef Z_NET_SRC_TCP_SERVER_H
#define Z_NET_SRC_TCP_SERVER_H

#include <map>
#include <string>
#include "defines.h"
#include "tcp_connection.h"

NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(net)

class Acceptor;
class EventLoop;
class EventLoopThreadPool;

class TcpServer
{
public:
    typedef std::map<std::string, TcpConnectionPtr> ConnectionMap;

    TcpServer(EventLoop* loop, const NetAddress& listenAddr, const std::string& name);
    ~TcpServer();

    void start();
    EventLoop* getEventLoop();

    void setThreadNum(int num);
    void setConnectionCallback(const ConnectionCallback& cb);
    void setMessageCallback(const MessageCallback& cb);

private:
    void newConnection(int sockfd, const NetAddress& peerAddr);
    void removeConnection(const TcpConnectionPtr& conn);
    void removeConnectionInLoop(const TcpConnectionPtr& conn);

private:
    EventLoop* loop_;  // acceptor loop
    const std::string ipPort_;
    std::string name_;
    bool started_;
    std::unique_ptr<Acceptor> acceptor_;
    std::shared_ptr<EventLoopThreadPool> threadPool_;
    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    WriteCompleteCallback writeCompleteCallback_;

    int nextConnId_;
    ConnectionMap connections_;
};

NAMESPACE_END(net)
NAMESPACE_END(z)

#endif  // Z_NET_SRC_TCP_SERVER_H