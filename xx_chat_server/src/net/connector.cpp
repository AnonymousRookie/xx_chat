#include <string.h>
#include "connector.h"
#include "event_loop.h"
#include "sockets_ops.h"
#include "channel.h"
#include "socket.h"
#include "logger.h"

NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(net)

Connector::Connector(EventLoop* loop, const NetAddress& serverAddr)
    : loop_(loop)
    , serverAddr_(serverAddr)
{

}

Connector::~Connector()
{
    assert(!channel_);
}

void Connector::setNewConnectionCallback(const NewConnectionCallback& cb)
{
    newConnectionCallback_ = cb;
}

void Connector::start()
{
    connect_ = true;
    loop_->runInLoop(std::bind(&Connector::startInLoop, this));
}

void Connector::reStart()
{

}

void Connector::stop()
{
    connect_ = false;
    loop_->runInLoop(std::bind(&Connector::stopInLoop, this));
}

void Connector::setState(States state)
{
    state_ = state;
}

const char* Connector::getState(States state)
{
    switch (state)
    {
    case kDisconnected:
        return "kDisconnected";
    case kConnecting:
        return "kConnecting";
    case kConnected:
        return "kConnected";
    default:
        return "unexpected state";
    }
}

void Connector::startInLoop()
{
    loop_->assertInLoopThread();
    assert(state_ == kDisconnected);
    if (connect_) {
        connect();
    }
}

void Connector::stopInLoop()
{
    loop_->assertInLoopThread();
    if (state_ == kConnecting) {
        setState(kDisconnected);
        int sockfd = removeAndResetChannel();
    }
}

void Connector::connect()
{
    Socket sock;
    int sockfd = sock.getSockfd();
    int ret = ::connect(sockfd, (const struct sockaddr*)(&serverAddr_.getSockAddr()), sizeof(struct sockaddr_in));
    int flag = (0 == ret ? 0 : errno);
    switch(flag)
    {
        case 0:
        case EINPROGRESS:
        case EINTR:
            connecting(sockfd);
            break;

        case EAGAIN:
            retry(sockfd);
            break;

        default:
            printf("connect error!\n");
            sockets::close(sockfd);
            break;
    }
}

void Connector::connecting(int sockfd)
{
    setState(kConnecting);
    assert(!channel_);
    channel_.reset(new Channel(loop_, sockfd));
    channel_->setWriteCallback(std::bind(&Connector::handleWrite, this));
    channel_->setErrorCallback(std::bind(&Connector::handleError, this));
    channel_->enableWriting();
}

void Connector::handleWrite()
{
    if (state_ == kConnecting)
    {
        int sockfd = removeAndResetChannel();
        int err = sockets::getSocketError(sockfd);
        if (err)
        {
            LOG_WARN("Connector::handleWrite - SO_ERROR = %d", err);
            retry(sockfd);
        }
        else if (sockets::isSelfConnect(sockfd))
        {
            LOG_WARN("Connector::handleWrite - Self connect");
            retry(sockfd);
        }
        else
        {
            setState(kConnected);
            if (connect_)
            {
                newConnectionCallback_(sockfd);
            }
            else
            {
                sockets::close(sockfd);
            }
        }
    }
    else
    {
        assert(state_ == kDisconnected);
    }
}

void Connector::handleError()
{
    LOG_ERROR("Connector::handleError state=%s", getState(state_));
    if (state_ == kConnecting)
    {
        int sockfd = removeAndResetChannel();
        int err = sockets::getSocketError(sockfd);
        LOG_INFO("sockfd = %d, err = %d, error info = %s", sockfd, err, strerror(err));
        retry(sockfd);
    }
}

void Connector::retry(int sockfd)
{
    sockets::close(sockfd);
    setState(kDisconnected);

    // fixme: 定时重试
}

int Connector::removeAndResetChannel()
{
    channel_->disableAll();
    channel_->remove();
    int sockfd = channel_->fd();
    return sockfd;
}

void Connector::resetChannel()
{
    channel_.reset();
}

NAMESPACE_END(net)
NAMESPACE_END(z)