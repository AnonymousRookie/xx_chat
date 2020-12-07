#include "tcp_client.h"
#include "event_loop.h"
#include "socket.h"
#include "connector.h"
#include "sockets_ops.h"
#include "logger.h"

NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(net)

void removeConn(EventLoop* loop, const TcpConnectionPtr& conn)
{
    loop->queueInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
}

TcpClient::TcpClient(EventLoop* loop, const NetAddress& serverAddr, const std::string& name)
    : loop_(loop)
    , connector_(new Connector(loop, serverAddr))
    , name_(name)
    , connectionCallback_(defaultConnectionCallback)
    , messageCallback_(defaultMessageCallback)
    , connect_(true)
    , nextConnId_(1)
{
    connector_->setNewConnectionCallback(
        std::bind(&TcpClient::newConnection, this, std::placeholders::_1));
}

TcpClient::~TcpClient()
{
    TcpConnectionPtr conn;
    bool unique = false;
    {
        std::unique_lock<std::mutex> lock(mutex_);
        unique = connection_.unique();
        conn = connection_;
    }
    if (conn)
    {
        assert(loop_ == conn->getLoop());
        CloseCallback cb = std::bind(&removeConn, loop_, _1);
        loop_->runInLoop(
            std::bind(&TcpConnection::setCloseCallback, conn, cb));
        if (unique)
        {
            conn->forceClose();
        }
    }
    else
    {
        connector_->stop();
    }
}

void TcpClient::connect()
{
    connect_ = true;
    connector_->start();
}

void TcpClient::disconnect()
{
    connect_ = false;
    {
        std::unique_lock<std::mutex> lock(mutex_);
        if (connection_)
        {
            connection_->shutdown();
        }
    }
}

void TcpClient::stop()
{
    connect_ = false;
    connector_->stop();
}

void TcpClient::newConnection(int sockfd)
{
    loop_->assertInLoopThread();
    NetAddress peerAddr(sockets::getPeerAddr(sockfd));
    char buf[32] = {0};
    snprintf(buf, sizeof(buf), ":%s#%d", peerAddr.toIpPort().c_str(), nextConnId_);
    ++nextConnId_;
    std::string connName = name_ + buf;

    NetAddress localAddr(sockets::getLocalAddr(sockfd));
    TcpConnectionPtr conn(new TcpConnection(loop_, connName, sockfd, localAddr, peerAddr));
    conn->setConnectionCallback(connectionCallback_);
    conn->setMessageCallback(messageCallback_);
    conn->setWriteCompleteCallback(writeCompleteCallback_);
    conn->setCloseCallback(std::bind(&TcpClient::removeConnection, this, std::placeholders::_1));
    
    {
        std::unique_lock<std::mutex> lock(mutex_);
        connection_ = conn;
    }

    conn->connectEstablished();
}

void TcpClient::removeConnection(const TcpConnectionPtr& conn)
{
    loop_->assertInLoopThread();
    assert(loop_ == conn->getLoop());

    {
        std::unique_lock<std::mutex> lock(mutex_);
        assert(connection_ == conn);
        connection_.reset();
    }

    loop_->queueInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
}

NAMESPACE_END(net)
NAMESPACE_END(z)