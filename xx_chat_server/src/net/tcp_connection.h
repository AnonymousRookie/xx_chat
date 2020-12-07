#ifndef Z_NET_SRC_TCP_CONNECTION_H
#define Z_NET_SRC_TCP_CONNECTION_H

#include <map>
#include <vector>
#include <memory>
#include "defines.h"
#include "callbacks.h"
#include "net_address.h"
#include "buffer.h"

NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(net)

class Channel;
class EventLoop;
class Socket;

typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;

class TcpConnection : public std::enable_shared_from_this<TcpConnection>
{
public:
    TcpConnection(EventLoop* loop, 
                  const std::string& name, 
                  int sockfd, 
                  const NetAddress& localAddr, 
                  const NetAddress& peerAddr);
    ~TcpConnection();

    EventLoop* getLoop() { return loop_; }
    const std::string& name() { return name_; }
    const NetAddress& localAddress() { return localAddr_; }
    const NetAddress& peerAddress() { return peerAddr_; }
    bool connected() { return state_ == State::kConnected; }
    bool disconnected() { return state_ == State::kDisconnected; }

    void send(const void* message, int len);
	void send(const std::string& message);
	void send(Buffer* message);

    void shutdown();

    void forceClose();
    void forceCloseWithDelay(double seconds);
    void setTcpNoDelay(bool on);

    void startRead();
    void stopRead();

    void setConnectionCallback(const ConnectionCallback& cb) {
         connectionCallback_ = cb; 
    }

    void setMessageCallback(const MessageCallback& cb) {
         messageCallback_ = cb; 
    }

    void setWriteCompleteCallback(const WriteCompleteCallback& cb) { 
        writeCompleteCallback_ = cb; 
    }

    void setHighWaterMarkCallback(const HighWaterMarkCallback& cb, size_t highWaterMark) { 
        highWaterMarkCallback_ = cb; highWaterMark_ = highWaterMark; 
    }

    void setCloseCallback(const CloseCallback& cb) { 
        closeCallback_ = cb; 
    }

    Buffer* inputBuffer() { 
        return &inputBuffer_; 
    }

    Buffer* outputBuffer() { 
        return &outputBuffer_; 
    }

    void connectEstablished();
    void connectDestroyed();

private:
    enum State
    {
        kDisconnected = 0, 
        kConnecting, 
        kConnected, 
        kDisconnecting,
    };

    void handleRead(Timestamp receiveTime);
    void handleWrite();
    void handleClose();
    void handleError();

    void sendInLoop(const std::string& message);
    void sendInLoop(const void* message, size_t len);
    void shutdownInLoop();

    void forceCloseInLoop();
    void setState(State s) { state_ = s; }
    const char* stateToString() const;

private:
    EventLoop* loop_;
    const std::string name_;
    State state_;

    std::unique_ptr<Socket> socket_;
    std::unique_ptr<Channel> channel_;
    const NetAddress localAddr_;
    const NetAddress peerAddr_;
    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    WriteCompleteCallback writeCompleteCallback_;
    HighWaterMarkCallback highWaterMarkCallback_;
    CloseCallback closeCallback_;
    size_t highWaterMark_;
    Buffer inputBuffer_;
    Buffer outputBuffer_; 
};

NAMESPACE_END(net)
NAMESPACE_END(z)

#endif  // Z_NET_SRC_TCP_CONNECTION_H