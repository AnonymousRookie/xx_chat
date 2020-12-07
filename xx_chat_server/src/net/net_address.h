#ifndef Z_NET_SRC_NET_ADDRESS_H
#define Z_NET_SRC_NET_ADDRESS_H

#include <string>
#include <cstdint>
#include <netinet/in.h>
#include "defines.h"

NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(net)

class NetAddress
{
public:
    NetAddress();
    NetAddress(const std::string& ip, uint16_t port);

    NetAddress(const struct sockaddr_in& addr) : addr_(addr) { }

    const struct sockaddr_in& getSockAddr() const;
    void setSockAddr(const struct sockaddr_in& addr);

    std::string toIp() const;
    std::string toIpPort() const;

private:
    struct sockaddr_in addr_;
};

NAMESPACE_END(net)
NAMESPACE_END(z)

#endif  // Z_NET_SRC_NET_ADDRESS_H