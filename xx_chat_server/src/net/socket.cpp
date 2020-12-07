#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include "socket.h"
#include "net_address.h"
#include "sockets_ops.h"
#include "logger.h"

NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(net)

Socket::Socket()
{
    int sockfd = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd < 0) {
        printf("socket error!\n");
    }
    setNonBlockAndCloseOnExec(sockfd);
    sockfd_ = sockfd;
}

Socket::Socket(int sockfd)
    : sockfd_(sockfd)
{

}

Socket::~Socket()
{

}

int Socket::getSockfd()
{
    return sockfd_;
}

void Socket::bind(const NetAddress& localaddr)
{
    int ret = ::bind(sockfd_, (const struct sockaddr*)(&localaddr.getSockAddr()), sizeof(struct sockaddr_in));
    if (ret < 0) {
        printf("bind error!\n");
    }
}

void Socket::listen()
{
    int ret = ::listen(sockfd_, SOMAXCONN);
    if (ret < 0) {
        printf("listen error!\n");
    }
}

int Socket::accept(NetAddress* peeraddr)
{
    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);
    int connfd = ::accept(sockfd_, (struct sockaddr*)&addr, &addrlen);
    if (connfd >= 0) {
        peeraddr->setSockAddr(addr);
    }
    setNonBlockAndCloseOnExec(connfd);
    return connfd;
}

void Socket::shutdownWrite()
{
    sockets::shutdownWrite(sockfd_);
}

void Socket::setTcpNoDelay(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(sockfd_, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(optval));
}

void Socket::setReuseAddr(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
}

void Socket::setKeepAlive(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(sockfd_, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval));
}

void Socket::setNonBlockAndCloseOnExec(int sockfd)
{
    // non-block
    int flags = ::fcntl(sockfd, F_GETFL, 0);
    flags |= O_NONBLOCK;
    int ret = ::fcntl(sockfd, F_SETFL, flags);
    if (ret < 0) {
        printf("fcntl error!\n");
    }

    // close-on-exec
    flags = ::fcntl(sockfd, F_GETFD, 0);
    flags |= FD_CLOEXEC;
    ret = ::fcntl(sockfd, F_SETFD, flags);
    if (ret < 0) {
        printf("fcntl error!\n");
    }
}

NAMESPACE_END(net)
NAMESPACE_END(z)
