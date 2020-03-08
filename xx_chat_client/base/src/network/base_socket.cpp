/**
 * Copyright 2018-2019, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: BaseSocket
 */

#include <unordered_map>
#include <cstring>
#include "base_socket.h"
#include "event_dispatch.h"

typedef std::unordered_map<net_handle_t, BaseSocketPtr> SocketMap;
SocketMap gSocketMap_;

void AddBaseSocket(const BaseSocketPtr& socket)
{
    gSocketMap_.insert(std::make_pair(static_cast<net_handle_t>(socket->GetSocket()), socket));
}

void RemoveBaseSocket(const BaseSocketPtr& socket)
{
    gSocketMap_.erase(static_cast<net_handle_t>(socket->GetSocket()));
}

BaseSocketPtr FindBaseSocket(net_handle_t fd)
{
    BaseSocketPtr socket = nullptr;
    auto iter = gSocketMap_.find(fd);
    if (iter != gSocketMap_.end()) {
        socket = iter->second;
    }
    return socket;
}

BaseSocket::BaseSocket()
    : socket_(INVALID_SOCKET)
    , state_(SocketState::SOCKET_STATE_IDLE)
{
}

BaseSocket::~BaseSocket()
{
}

int BaseSocket::Listen(const char* serverIp, uint16_t port, const NewConnectionCallback& cb)
{
    localIp_ = serverIp;
    localPort_ = port;
    newConnectionCallback_ = cb;

    socket_ = socket(AF_INET, SOCK_STREAM, 0);
    if (INVALID_SOCKET == socket_) {
        printf("socket failed, errCode = %d\n", GetErrorCode());
        return NETLIB_ERROR;
    }

    SetReuseAddr(socket_);
    SetNonblock(socket_);

    sockaddr_in servAddr;
    SetAddr(serverIp, port, &servAddr);

    int ret = ::bind(socket_, (sockaddr*)&servAddr, sizeof(servAddr));
    if (SOCKET_ERROR == ret) {
        printf("bind failed, errCode = %d\n", GetErrorCode());
        closesocket(socket_);
        return NETLIB_ERROR;
    }

    ret = listen(socket_, 64);
    if (SOCKET_ERROR == ret) {
        printf("listen failed, errCode = %d\n", GetErrorCode());
        closesocket(socket_);
        return NETLIB_ERROR;
    }

    SetState(SocketState::SOCKET_STATE_LISTENING);
    printf("BaseSocket::Listen on %s:%d\n", serverIp, port);

    AddBaseSocket(shared_from_this());
    EventDispatch::Instance()->AddEvent(socket_, SOCKET_READ | SOCKET_EXCEP);

    return NETLIB_OK;
}

net_handle_t BaseSocket::Connect(const char* serverIp, uint16_t port, const MessageCallback& cb)
{
    remoteIp_ = serverIp;
    remotePort_ = port;
    messageCallback_ = cb;

    socket_ = socket(AF_INET, SOCK_STREAM, 0);
    if (INVALID_SOCKET == socket_) {
        printf("socket failed, errCode=%d", GetErrorCode());
        return NETLIB_INVALID_HANDLE;
    }

    SetNonblock(socket_);
    SetNoDelay(socket_);

    sockaddr_in servAddr;
    SetAddr(serverIp, port, &servAddr);
    int ret = connect(socket_, (sockaddr*)&servAddr, sizeof(servAddr));
    if ( (SOCKET_ERROR == ret) && (!IsBlock(GetErrorCode())) ) {
        printf("connect failed, errCode=%d", GetErrorCode());
        closesocket(socket_);
        return NETLIB_INVALID_HANDLE;
    }
    state_ = SocketState::SOCKET_STATE_CONNECTING;
    AddBaseSocket(shared_from_this());
    EventDispatch::Instance()->AddEvent(socket_, SOCKET_ALL);
    
    return static_cast<net_handle_t>(socket_);
}

int BaseSocket::Send(void* buf, int len)
{
    if (state_ != SocketState::SOCKET_STATE_CONNECTED) {
        return NETLIB_ERROR;
    }

    int ret = send(socket_, (char*)buf, len, 0);
    if (SOCKET_ERROR == ret) {
        int errCode = GetErrorCode();
        if (IsBlock(errCode)) {
#ifdef _WIN32
            EventDispatch::Instance()->AddEvent(socket_, SOCKET_WRITE);
#endif
            ret = 0;
        }
        else {
            printf("send failed, error code: %d", errCode);
        }
    }
    return ret;
}

int BaseSocket::Recv(void* buf, int len)
{
    return recv(socket_, (char*)buf, len, 0);
}

int BaseSocket::Close()
{
    EventDispatch::Instance()->RemoveEvent(socket_, SOCKET_ALL);
    RemoveBaseSocket(shared_from_this());
    closesocket(socket_);
    return 0;
}

void BaseSocket::OnRead()
{
    if (state_ == SocketState::SOCKET_STATE_LISTENING) {
        AcceptNewSocket();
    } else {
        u_long avail = 0;
        if ((ioctlsocket(socket_, FIONREAD, &avail) == SOCKET_ERROR) || (avail == 0)) {
            messageCallback_(NETLIB_MSG_CLOSE, (net_handle_t)socket_);
        } else {
            messageCallback_(NETLIB_MSG_READ, (net_handle_t)socket_);
        }
    }
}

void BaseSocket::OnWrite()
{
#ifdef _WIN32
    EventDispatch::Instance()->RemoveEvent(socket_, SOCKET_WRITE);
#endif

    if (state_ == SocketState::SOCKET_STATE_CONNECTING) {
        int error = 0;
        socklen_t len = sizeof(error);
#ifdef _WIN32
        getsockopt(socket_, SOL_SOCKET, SO_ERROR, (char*)&error, &len);
#else
        getsockopt(socket_, SOL_SOCKET, SO_ERROR, (void*)&error, &len);
#endif
        if (error) {
            messageCallback_(NETLIB_MSG_CLOSE, (net_handle_t)socket_);
        } else {
            state_ = SocketState::SOCKET_STATE_CONNECTED;
            messageCallback_(NETLIB_MSG_CONFIRM, (net_handle_t)socket_);
        }
    } else {
        messageCallback_(NETLIB_MSG_WRITE, (net_handle_t)socket_);
    }
}

void BaseSocket::OnClose()
{
    state_ = SocketState::SOCKET_STATE_CLOSING;
    messageCallback_(NETLIB_MSG_CLOSE, (net_handle_t)socket_);
}

void BaseSocket::SetSendBufSize(uint32_t sendSize)
{
#ifdef _WIN32
    int ret = setsockopt(socket_, SOL_SOCKET, SO_SNDBUF, (char*)&sendSize, 4);
    if (SOCKET_ERROR == ret) {
        printf("set SO_SNDBUF failed for fd=%d", socket_);
        return;
    }
    socklen_t len = 4;
    int size = 0;
    getsockopt(socket_, SOL_SOCKET, SO_SNDBUF, (char*)&size, &len);
    printf("socket=%d send_buf_size=%d", socket_, size);
#else
    int ret = setsockopt(socket_, SOL_SOCKET, SO_SNDBUF, (void*)&sendSize, 4);
    if (SOCKET_ERROR == ret) {
        printf("set SO_SNDBUF failed for fd=%d", socket_);
        return;
    }
    socklen_t len = 4;
    int size = 0;
    getsockopt(socket_, SOL_SOCKET, SO_SNDBUF, (void*)&size, &len);
    printf("socket=%d send_buf_size=%d", socket_, size);
#endif
}

void BaseSocket::SetRecvBufSize(uint32_t recvSize)
{
#ifdef _WIN32
    int ret = setsockopt(socket_, SOL_SOCKET, SO_RCVBUF, (char*)&recvSize, 4);
    if (SOCKET_ERROR == ret) {
        printf("set SO_RCVBUF failed for fd=%d", socket_);
        return;
    }
    socklen_t len = 4;
    int size = 0;
    getsockopt(socket_, SOL_SOCKET, SO_RCVBUF, (char*)&size, &len);
    printf("socket=%d recv_buf_size=%d", socket_, size);
#else
    int ret = setsockopt(socket_, SOL_SOCKET, SO_RCVBUF, (void*)&recvSize, 4);
    if (SOCKET_ERROR == ret) {
        printf("set SO_RCVBUF failed for fd=%d", socket_);
        return;
    }
    socklen_t len = 4;
    int size = 0;
    getsockopt(socket_, SOL_SOCKET, SO_RCVBUF, (void*)&size, &len);
    printf("socket=%d recv_buf_size=%d", socket_, size);
#endif
}

int BaseSocket::GetErrorCode()
{
#ifdef _WIN32
    return WSAGetLastError();
#else
    return errno;
#endif
}

bool BaseSocket::IsBlock(int error_code)
{
#ifdef _WIN32
    return ( (error_code == WSAEINPROGRESS) || (error_code == WSAEWOULDBLOCK) );
#else
    return ( (error_code == EINPROGRESS) || (error_code == EWOULDBLOCK) );
#endif
}

void BaseSocket::SetNonblock(SOCKET fd)
{
#ifdef _WIN32
    u_long nonblock = 1;
    int ret = ioctlsocket(fd, FIONBIO, &nonblock);
#else
    int ret = fcntl(fd, F_SETFL, O_NONBLOCK | fcntl(fd, F_GETFL));
#endif
    if (SOCKET_ERROR == ret) {
        printf("SetNonblock failed, errCode=%d", GetErrorCode());
    }
}

void BaseSocket::SetReuseAddr(SOCKET fd)
{
    int reuse = 1;
#ifdef _WIN32
    int ret = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse, sizeof(reuse));
#else
    int ret = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (void*)&reuse, sizeof(reuse));
#endif
    if (SOCKET_ERROR == ret) {
        printf("SetReuseAddr failed, errCode=%d", GetErrorCode());
    }
}

void BaseSocket::SetNoDelay(SOCKET fd)
{
    int nodelay = 1;
#ifdef _WIN32
    int ret = setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char*)&nodelay, sizeof(nodelay));
#else
    int ret = setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (void*)&nodelay, sizeof(nodelay));
#endif
    if (ret == SOCKET_ERROR) {
        printf("SetNoDelay failed, errCode=%d", GetErrorCode());
    }
}

void BaseSocket::SetAddr(const char* ip, const uint16_t port, sockaddr_in* pAddr)
{
    memset(pAddr, 0, sizeof(sockaddr_in));
    pAddr->sin_family = AF_INET;
    pAddr->sin_port = htons(port);
    pAddr->sin_addr.s_addr = inet_addr(ip);
    if (pAddr->sin_addr.s_addr == INADDR_NONE) {
        hostent* host = gethostbyname(ip);
        if (!host) {
            printf("gethostbyname failed, ip=%s", ip);
            return;
        }
        pAddr->sin_addr.s_addr = *(uint32_t*)host->h_addr;
    }
}

void BaseSocket::AcceptNewSocket()
{
    SOCKET fd = 0;
    sockaddr_in peerAddr;
    socklen_t addr_len = sizeof(sockaddr_in);
    char ip_str[64] = {0};
    while ( (fd = accept(socket_, (sockaddr*)&peerAddr, &addr_len)) != INVALID_SOCKET ) 
    {
        BaseSocketPtr pSocket = std::make_shared<BaseSocket>();
        uint32_t ip = ntohl(peerAddr.sin_addr.s_addr);
        uint16_t port = ntohs(peerAddr.sin_port);

        snprintf(ip_str, sizeof(ip_str), "%d.%d.%d.%d", ip >> 24, (ip >> 16) & 0xFF, (ip >> 8) & 0xFF, ip & 0xFF);
        printf("AcceptNewSocket, socket=%d from %s:%d\n", fd, ip_str, port);

        pSocket->SetSocket(fd);
        pSocket->SetState(SocketState::SOCKET_STATE_CONNECTED);
        pSocket->SetRemoteIP(ip_str);
        pSocket->SetRemotePort(port);

        SetNoDelay(fd);
        SetNonblock(fd);
        AddBaseSocket(pSocket);
        EventDispatch::Instance()->AddEvent(fd, SOCKET_READ | SOCKET_EXCEP);
        newConnectionCallback_(NETLIB_MSG_CONNECT, (net_handle_t)fd);
    }
}
