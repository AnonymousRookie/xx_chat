#ifndef Z_NET_SRC_SOCKET_H
#define Z_NET_SRC_SOCKET_H

#include "defines.h"

NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(net)

class NetAddress;

class Socket
{
public:
    Socket();
    explicit Socket(int sockfd);
    Socket(const Socket& sock) = delete;
    Socket& operator=(const Socket& sock) = delete;
    ~Socket();

    int getSockfd();
    void bind(const NetAddress& localaddr);
    void listen();
    int accept(NetAddress* peeraddr);

    void shutdownWrite();

    void setTcpNoDelay(bool on);
    void setReuseAddr(bool on);
    void setKeepAlive(bool on);

private:
    void setNonBlockAndCloseOnExec(int sockfd);

private:
    int sockfd_;
};

NAMESPACE_END(net)
NAMESPACE_END(z)

#endif  // Z_NET_SRC_SOCKET_H