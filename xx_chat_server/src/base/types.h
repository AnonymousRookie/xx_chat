/**
 * Copyright 2018-2019, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: 基本类型定义
 */

#ifndef BASE_TYPES_H
#define BASE_TYPES_H

#include <functional>
#include <cstdint>

#ifdef _WIN32
    #include <WinSock2.h>
    #include <WinBase.h>
    #include <Windows.h>
#else
    #include <sys/epoll.h>
    #include <pthread.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <sys/ioctl.h>
    #include <netinet/in.h>
    #include <netinet/tcp.h>
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <errno.h>
    #include <unistd.h>
    #include <fcntl.h>
    #include <stdint.h>
    #include <signal.h>
    #include <unistd.h>
    #define closesocket close
    #define ioctlsocket ioctl
#endif

#ifdef _WIN32
    typedef int	socklen_t;
#else
    typedef int	SOCKET;
    typedef int BOOL;
    const int TRUE = 1;
    const int FALSE = 0;
    const int SOCKET_ERROR	= -1;
    const int INVALID_SOCKET = -1;
#endif

typedef unsigned char	uchar_t;
typedef int				net_handle_t;
typedef int				conn_handle_t;

enum {
	NETLIB_OK = 0,
	NETLIB_ERROR = -1
};

#define NETLIB_INVALID_HANDLE -1

enum
{
	NETLIB_MSG_CONNECT = 1,
	NETLIB_MSG_CONFIRM,
	NETLIB_MSG_READ,
	NETLIB_MSG_WRITE,
	NETLIB_MSG_CLOSE,
	NETLIB_MSG_TIMER,
    NETLIB_MSG_LOOP
};

const uint32_t INVALID_UINT32 = (uint32_t)-1;
const uint32_t INVALID_VALUE = 0;

typedef std::function<void(uint8_t msg, uint32_t handle)> NewConnectionCallback;
typedef std::function<void(uint8_t msg, uint32_t handle)> MessageCallback;
typedef std::function<void(uint8_t msg, uint32_t handle)> TimerCallback;
typedef std::function<void(uint8_t msg, uint32_t handle)> LoopCallback;


#endif  // BASE_TYPES_H