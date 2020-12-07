#pragma once

#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include "defines.h"

NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(net)

class Buffer;
class EventLoop;
class TcpServer;
class TcpConnection;

NAMESPACE_END(net)
NAMESPACE_END(z)

struct MsgServerInfo {
    std::string ip_addr;
    uint16_t	port;
    uint32_t	max_conn_cnt;
    uint32_t	cur_conn_cnt;
    std::string hostname;
};

class LoginServer final
{
public:
    LoginServer() = default;
    ~LoginServer() = default;

    LoginServer(const LoginServer& ls) = delete;
    LoginServer& operator=(const LoginServer& ls) = delete;

public:
    bool init(const char* ip, short port, z::net::EventLoop* loop);
    void uninit();

    void onConnected(std::shared_ptr<z::net::TcpConnection> conn);
    void onDisconnected(const std::shared_ptr<z::net::TcpConnection>& conn);

    void onMessage(const std::shared_ptr<z::net::TcpConnection>& conn, z::net::Buffer* buf, z::net::Timestamp time);

private:
    void handleClientData(const std::shared_ptr<z::net::TcpConnection>& conn, const char* data, int len);
    void handleMsgServerData(const std::shared_ptr<z::net::TcpConnection>& conn, const char* data, int len); 

private:
    std::unique_ptr<z::net::TcpServer> server_;
    std::vector<MsgServerInfo> msgServerInfos_;
};
