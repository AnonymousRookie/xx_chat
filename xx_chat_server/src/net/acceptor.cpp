#include "acceptor.h"
#include "event_loop.h"
#include "net_address.h"
#include "sockets_ops.h"
#include "logger.h"

NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(net)

Acceptor::Acceptor(EventLoop* loop, const NetAddress& listenAddr)
    : loop_(loop)
    , acceptChannel_(loop, acceptSocket_.getSockfd())
{
    acceptSocket_.setReuseAddr(true);
    acceptSocket_.bind(listenAddr);
    acceptChannel_.setReadCallback(std::bind(&Acceptor::handleRead, this));
}

Acceptor::~Acceptor()
{
    acceptChannel_.disableAll();
    acceptChannel_.remove();
}

void Acceptor::setNewConnectionCallback(const NewConnectionCallback& cb)
{
    newConnectionCallback_ = cb;
}

void Acceptor::listen()
{
    loop_->assertInLoopThread();
    listening_ = true;
    acceptSocket_.listen();
    acceptChannel_.enableReading();
}

bool Acceptor::listening()
{
    return listening_;
}

void Acceptor::handleRead()
{
    loop_->assertInLoopThread();
    NetAddress peerAddr;
    int connfd = acceptSocket_.accept(&peerAddr);
    if (connfd >= 0) 
    {
        if (newConnectionCallback_) {
            newConnectionCallback_(connfd, peerAddr);
        }
        else {
            z::net::sockets::close(connfd);
        }
    }
}

NAMESPACE_END(net)
NAMESPACE_END(z)