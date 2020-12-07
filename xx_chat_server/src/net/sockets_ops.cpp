#include <chrono>
#include <ctime>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>      // snprintf
#include <sys/socket.h>
#include <sys/uio.h>    // readv
#include <unistd.h>
#include <string.h>
#include "sockets_ops.h"
#include "z_endian.h"
#include "logger.h"

NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(net)

ssize_t sockets::read(int sockfd, void *buf, size_t count)
{
    return ::read(sockfd, buf, count);
}

ssize_t sockets::readv(int sockfd, const struct iovec *iov, int iovcnt)
{
    return ::readv(sockfd, iov, iovcnt);
}

ssize_t sockets::write(int sockfd, const void *buf, size_t count)
{
    return ::write(sockfd, buf, count);
}

void sockets::close(int sockfd)
{
    if (::close(sockfd) < 0)
    {
        printf("close error!\n");
    }
}

void sockets::shutdownWrite(int sockfd)
{
    if (::shutdown(sockfd, SHUT_WR) < 0)
    {
        printf("sockets::shutdownWrite!\n");
    }
}


struct sockaddr_in sockets::getLocalAddr(int sockfd)
{
    struct sockaddr_in localaddr = { 0 };
    memset(&localaddr, 0, sizeof(localaddr));
    socklen_t addrlen = static_cast<socklen_t>(sizeof(localaddr));
    ::getsockname(sockfd, (struct sockaddr*)(&localaddr), &addrlen);
    return localaddr;
}

struct sockaddr_in sockets::getPeerAddr(int sockfd)
{
    struct sockaddr_in peeraddr = { 0 };
    memset(&peeraddr, 0, sizeof(peeraddr));
    socklen_t addrlen = static_cast<socklen_t>(sizeof(peeraddr));
    ::getpeername(sockfd, (struct sockaddr*)(&peeraddr), &addrlen);
    return peeraddr;
}

void sockets::fromIpPort(const char* ip, uint16_t port, struct sockaddr_in* addr)
{
    addr->sin_family = AF_INET;
    addr->sin_port = hostToNetwork16(port);
    if (::inet_pton(AF_INET, ip, &addr->sin_addr) <= 0)
    {
        LOG_ERROR("sockets::fromIpPort error!");
    }
}

void sockets::toIpPort(char* buf, size_t size, const struct sockaddr_in& addr)
{
    ::inet_ntop(AF_INET, &addr.sin_addr, buf, static_cast<socklen_t>(size));
    size_t end = ::strlen(buf);
    uint16_t port = sockets::networkToHost16(addr.sin_port);
    snprintf(buf + end, size - end, ":%u", port);
}

void sockets::toIp(char* buf, size_t size, const struct sockaddr_in& addr)
{
    if (size >= sizeof(struct sockaddr_in))
        return;
    ::inet_ntop(AF_INET, &addr.sin_addr, buf, static_cast<socklen_t>(size));
}

int sockets::getSocketError(int sockfd)
{
    int optval;
    socklen_t optlen = static_cast<socklen_t>(sizeof(optval));

    if (::getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &optval, &optlen) < 0)
    {
        return errno;
    }
    else
    {
        return optval;
    }
}

bool sockets::isSelfConnect(int sockfd)
{
    struct sockaddr_in localaddr = getLocalAddr(sockfd);
    struct sockaddr_in peeraddr = getPeerAddr(sockfd);
    return localaddr.sin_port == peeraddr.sin_port && localaddr.sin_addr.s_addr == peeraddr.sin_addr.s_addr;
}

NAMESPACE_END(net)
NAMESPACE_END(z)