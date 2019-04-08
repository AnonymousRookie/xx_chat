/**
 * Copyright 2018-2019, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: BaseSocket
 */

#ifndef BASE_BASE_SOCKET_H
#define BASE_BASE_SOCKET_H

#include <memory>
#include <string>
#include "types.h"
#include "utils.h"

enum class SocketState {
    SOCKET_STATE_IDLE,
    SOCKET_STATE_LISTENING,
    SOCKET_STATE_CONNECTING,
    SOCKET_STATE_CONNECTED,
    SOCKET_STATE_CLOSING
};

class BaseSocket : public std::enable_shared_from_this<BaseSocket>
{
public:
    BaseSocket();
    virtual ~BaseSocket();

    SOCKET GetSocket() { return socket_; }
    void SetSocket(SOCKET fd) { socket_ = fd; }

    SocketState GetState() { return state_; }
    void SetState(SocketState state) { state_ = state; }

    void SetRemoteIP(char* ip) { remoteIp_ = ip; }
    const char* GetRemoteIP() { return remoteIp_.c_str(); }
    void SetRemotePort(uint16_t port) { remotePort_ = port; }
    uint16_t GetRemotePort() { return remotePort_; }

    void SetSendBufSize(uint32_t sendSize);
    void SetRecvBufSize(uint32_t recvSize);

    const char* GetLocalIP() { return localIp_.c_str(); }
    uint16_t GetLocalPort() { return localPort_; }

    void SetNewConnectionCallback(const NewConnectionCallback& cb) { newConnectionCallback_ = cb; }
    void SetMessageCallback(const MessageCallback& cb) { messageCallback_ = cb; }

    int Listen(const char* serverIp, uint16_t port, const NewConnectionCallback& cb);
    net_handle_t Connect(const char* serverIp, uint16_t port, const MessageCallback& cb);

    int Send(void* buf, int len);
    int Recv(void* buf, int len);
    int Close();

public:
    void OnRead();
    void OnWrite();
    void OnClose();

private:
    int GetErrorCode();
    bool IsBlock(int errorCode);

    void SetNonblock(SOCKET fd);
    void SetReuseAddr(SOCKET fd);
    void SetNoDelay(SOCKET fd);
    void SetAddr(const char* ip, const uint16_t port, sockaddr_in* addr);
    void AcceptNewSocket();

private:
    std::string localIp_;
    uint16_t localPort_;
    std::string remoteIp_;
    uint16_t remotePort_;

    NewConnectionCallback newConnectionCallback_;
    MessageCallback messageCallback_;

    SOCKET socket_;
    SocketState state_;
};

typedef std::shared_ptr<BaseSocket> BaseSocketPtr;
BaseSocketPtr FindBaseSocket(net_handle_t fd);


#endif  // BASE_BASE_SOCKET_H