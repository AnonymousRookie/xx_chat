#ifndef Z_NET_SRC_ACCEPTOR_H
#define Z_NET_SRC_ACCEPTOR_H

#include <string>
#include <functional>
#include "defines.h"
#include "socket.h"
#include "channel.h"

NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(net)

class EventLoop;
class NetAddress;

class Acceptor
{
public:
    typedef std::function<void (int sockfd, const NetAddress&)> NewConnectionCallback;

    Acceptor(EventLoop* loop, const NetAddress& listenAddr);
    ~Acceptor();

    void setNewConnectionCallback(const NewConnectionCallback& cb);
    void listen();
    bool listening();

private:
    void handleRead();

private:
    EventLoop* loop_;
    Socket acceptSocket_;
    Channel acceptChannel_;
    NewConnectionCallback newConnectionCallback_;
    bool listening_;
};

NAMESPACE_END(net)
NAMESPACE_END(z)

#endif  // Z_NET_SRC_ACCEPTOR_H