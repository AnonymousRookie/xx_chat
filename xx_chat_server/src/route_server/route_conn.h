/**
 * Copyright 2018-2019, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: MsgServer与RouteServer之间的连接
 */

#ifndef ROUTE_SERVER_ROUTE_CONN_H
#define ROUTE_SERVER_ROUTE_CONN_H

#include <memory>
#include "im_conn.h"

class RouteConn : public ImConn
{
public:
    RouteConn();
    virtual ~RouteConn();

	virtual void Close();
	virtual void OnConnect(net_handle_t handle);
	virtual void OnClose();
	virtual void OnTimer(uint64_t curr_tick);

	virtual void HandlePdu(std::shared_ptr<ImPdu> pPdu);

private:
	void HandleOnlineUserInfo(std::shared_ptr<ImPdu> pdu);

private:
	void SendPduToUser(uint32_t userId, std::shared_ptr<ImPdu> pdu, bool all);

private:
	bool master_;
};

typedef std::shared_ptr<RouteConn> RouteConnPtr;

void init_routeconn_timer_callback();


#endif  // ROUTE_SERVER_ROUTE_CONN_H