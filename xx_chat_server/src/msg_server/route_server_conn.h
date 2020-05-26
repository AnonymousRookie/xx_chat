/**
 * Copyright 2018-2019, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: MsgServer与RouteServer之间的连接
 */

#ifndef MSG_SERVER_ROUTE_SERVER_CONN_H
#define MSG_SERVER_ROUTE_SERVER_CONN_H

#include <memory>
#include "im_conn.h"
#include "serv_info.h"

class ImPdu;
class RouteServConn : public ImConn
{
public:
    RouteServConn();
    virtual ~RouteServConn();

    bool IsOpen() { return opened_; }
    uint64_t GetConnectTime() { return connectTime_; }
    void Connect(const char* serverIp, uint16_t serverPort, uint32_t serverIndex);

    virtual void Close();
	virtual void OnConfirm();
	virtual void OnClose();
	virtual void OnTimer(uint64_t currTick);
	virtual void HandlePdu(std::shared_ptr<ImPdu> pdu);

private:
	bool 		opened_;
	uint32_t	serverIndex_;
	uint64_t	connectTime_;
};

typedef std::shared_ptr<RouteServConn> RouteServConnPtr;

void init_route_serv_conn(ServerListType* server_list);
bool is_route_server_available();
void send_to_all_route_server(std::shared_ptr<ImPdu> pPdu);


#endif  // MSG_SERVER_ROUTE_SERVER_CONN_H