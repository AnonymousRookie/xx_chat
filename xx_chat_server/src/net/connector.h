#ifndef Z_NET_SRC_CONNECTOR_H
#define Z_NET_SRC_CONNECTOR_H

#include <functional>
#include <memory>
#include <string>
#include "defines.h"
#include "net_address.h"

NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(net)

class Channel;
class EventLoop;

class Connector : public std::enable_shared_from_this<Connector>
{
public:
    typedef std::function<void (int sockfd)> NewConnectionCallback;

    Connector(EventLoop* loop, const NetAddress& serverAddr);
    ~Connector();

    void setNewConnectionCallback(const NewConnectionCallback& cb);

    void start();
    void reStart();
    void stop();

    const NetAddress& serverAddress() const { return serverAddr_; }

private:
    enum States {
        kDisconnected = 0,
        kConnecting,
        kConnected,
    };

    enum RetryDelayTime {
        kInitRetryDelayMs = 500,       // 500ms
        kMaxRetryDelayMs = 30 * 1000,  // 30s
    };

    void setState(States state);
    const char* getState(States state);

    void startInLoop();
    void stopInLoop();

    void connect();
    void connecting(int sockfd);
    void handleWrite();
    void handleError();
    void retry(int sockfd);
    int removeAndResetChannel();
    void resetChannel();

    EventLoop* loop_;
    NetAddress serverAddr_;
    bool connect_ = false;
    States state_ = kDisconnected;
    std::unique_ptr<Channel> channel_;
    NewConnectionCallback newConnectionCallback_;
    int retryDelayMs_ = kInitRetryDelayMs;
};

NAMESPACE_END(net)
NAMESPACE_END(z)

#endif  // Z_NET_SRC_CONNECTOR_H