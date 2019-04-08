/**
 * Copyright 2018-2019, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: UserInfo
 */

#ifndef ROUTE_SERVER_USER_INFO_H
#define ROUTE_SERVER_USER_INFO_H

#include <memory>
#include <set>
#include <map>
#include "types.h"
#include "route_conn.h"

class UserInfo
{
    typedef std::set<RouteConnPtr> RouteConnSet;

public:
    UserInfo();
    ~UserInfo();

    uint32_t GetStatus();

    void AddRouteConn(RouteConnPtr conn) { routeConnSet_.insert(conn); }
    void RemoveRouteConn(RouteConnPtr conn) { routeConnSet_.erase(conn); }
    void ClearRouteConn() { routeConnSet_.clear(); }
    RouteConnSet* GetRouteConn() { return &routeConnSet_; }

    bool FindRouteConn(RouteConnPtr conn);
    uint32_t GetCountByClientType(uint32_t clientType);

    uint32_t GetRouteConnCount() { return routeConnSet_.size(); }

    void AddClientType(uint32_t clientType);
    void RemoveClientType(uint32_t clientType);

private:
    typedef std::map<uint32_t/*clientType*/, uint32_t/*count*/> ClientTypeList;
    RouteConnSet routeConnSet_;
    ClientTypeList clientTypeList_;
};

typedef std::shared_ptr<UserInfo> UserInfoPtr;


#endif  // ROUTE_SERVER_USER_INFO_H