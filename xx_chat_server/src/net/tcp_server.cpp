#include "tcp_server.h"
#include "acceptor.h"
#include "event_loop.h"
#include "event_loop_thread_pool.h"
#include "sockets_ops.h"
#include "logger.h"

NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(net)

TcpServer::TcpServer(EventLoop* loop, const NetAddress& listenAddr, const std::string& name)
    : loop_(loop)
    , ipPort_(listenAddr.toIpPort())
    , name_(name)
    , acceptor_(new Acceptor(loop, listenAddr))
    , threadPool_(new EventLoopThreadPool(loop, name))
    , connectionCallback_(defaultConnectionCallback)
    , messageCallback_(defaultMessageCallback)
    , started_(false)
    , nextConnId_(1)
{
    acceptor_->setNewConnectionCallback(std::bind(&TcpServer::newConnection, this, _1, _2));
}

TcpServer::~TcpServer()
{
    loop_->assertInLoopThread();

    for (auto& item : connections_)
    {
        TcpConnectionPtr conn(item.second);
        item.second.reset();
        conn->getLoop()->runInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
    }
}

void TcpServer::start()
{
    Z_CHECK(!started_);
    started_ = true;

    threadPool_->start();
    assert(!acceptor_->listening());
    loop_->runInLoop(std::bind(&Acceptor::listen, acceptor_.get()));
}

EventLoop* TcpServer::getEventLoop()
{
    return loop_;
}

void TcpServer::setThreadNum(int num)
{
    assert(num >= 0);
    threadPool_->setThreadNum(num);
}

void TcpServer::setConnectionCallback(const ConnectionCallback& cb)
{
    connectionCallback_ = cb;
}

void TcpServer::setMessageCallback(const MessageCallback& cb)
{
    messageCallback_ = cb;
}

void TcpServer::newConnection(int sockfd, const NetAddress& peerAddr)
{
    loop_->assertInLoopThread();
    EventLoop* ioLoop = threadPool_->getNextLoop();
    char buf[64];
    snprintf(buf, sizeof buf, "-%s#%d", ipPort_.c_str(), nextConnId_);
    ++nextConnId_;
    std::string connName = name_ + buf;

    LOG_INFO("TcpServer::newConnection [%s] - new connection [%s] from %s", name_.c_str(), connName.c_str(), peerAddr.toIpPort().c_str());  
    
    NetAddress localAddr(sockets::getLocalAddr(sockfd));
    TcpConnectionPtr conn(new TcpConnection(ioLoop,
                                            connName,
                                            sockfd,
                                            localAddr,
                                            peerAddr));
    connections_[connName] = conn;
    conn->setConnectionCallback(connectionCallback_);
    conn->setMessageCallback(messageCallback_);
    conn->setWriteCompleteCallback(writeCompleteCallback_);
    conn->setCloseCallback(
        std::bind(&TcpServer::removeConnection, this, _1)); 
    ioLoop->runInLoop(std::bind(&TcpConnection::connectEstablished, conn));
}

void TcpServer::removeConnection(const TcpConnectionPtr& conn)
{
    loop_->runInLoop(std::bind(&TcpServer::removeConnectionInLoop, this, conn));
}

void TcpServer::removeConnectionInLoop(const TcpConnectionPtr& conn)
{
    loop_->assertInLoopThread();
    LOG_INFO("TcpServer::removeConnectionInLoop [%s] - connection %s", name_.c_str(), conn->name().c_str());
    size_t n = connections_.erase(conn->name());
    (void)n;
    assert(n == 1);
    EventLoop* ioLoop = conn->getLoop();
    ioLoop->queueInLoop(
        std::bind(&TcpConnection::connectDestroyed, conn));
}

NAMESPACE_END(net)
NAMESPACE_END(z)