#ifndef Z_NET_SRC_SOCKETS_OPS_H
#define Z_NET_SRC_SOCKETS_OPS_H

#include <map>
#include <vector>
#include <arpa/inet.h>
#include "defines.h"

NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(net)
NAMESPACE_BEGIN(sockets)

ssize_t read(int sockfd, void *buf, size_t count);
ssize_t readv(int sockfd, const struct iovec *iov, int iovcnt);
ssize_t write(int sockfd, const void *buf, size_t count);
void close(int sockfd);
void shutdownWrite(int sockfd);

struct sockaddr_in getLocalAddr(int sockfd);
struct sockaddr_in getPeerAddr(int sockfd);

void fromIpPort(const char* ip, uint16_t port, struct sockaddr_in* addr);
void toIpPort(char* buf, size_t size, const struct sockaddr_in& addr);
void toIp(char* buf, size_t size, const struct sockaddr_in& addr);

int getSocketError(int sockfd);
bool isSelfConnect(int sockfd);

NAMESPACE_END(sockets)
NAMESPACE_END(net)
NAMESPACE_END(z)

#endif  // Z_NET_SRC_SOCKETS_OPS_H