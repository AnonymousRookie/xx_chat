/**
 * Copyright 2019-2020, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: 连接login_server
 */

#include <string>
#include <memory>
#include "login_win.h"
#include "util\string_util.h"
#include "network\net_lib.h"
#include "util\logger.h"
#include "rapidjson\document.h"
#include "..\core\http_client.h"
#include "login_http_server_operation.h"


NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(login)

LoginHttpServerOperation::LoginHttpServerOperation(z::core::OperationDelegate callback, DoLoginServerParam& param)
    : IHttpOperation(callback)
    , loginServerParam_(param)
{
}

void LoginHttpServerOperation::ProcessOperation()
{
    std::string url = "http://127.0.0.1:8080/msg_server";
    auto doLoginServerParam = std::make_shared<DoLoginServerParam>();

    std::string response;
    z::core::HttpClient httpClient;
    CURLcode ret = httpClient.Get(url, &response);
    if (ret != CURLE_OK) {
        SyncCallback(doLoginServerParam);
        return;
    }
    rapidjson::Document document;
    document.Parse(response.c_str());

    try {
        uint32_t code = document["code"].GetUint();
        if (code != 0) {
            std::string msg = document["msg"].GetString();
            LOG_WARN("login login_server failed! err: %s", msg.c_str());
            SyncCallback(doLoginServerParam);
            return;
        }
        // 连接成功, 得到msg_server的地址
        doLoginServerParam->msgServerIp = document["ip"].GetString();
        doLoginServerParam->msgServerPort = document["port"].GetUint();
        doLoginServerParam->result = LOGIN_LOGIN_SERVER_SUCCESS;
        SyncCallback(doLoginServerParam);
    }
    catch (std::runtime_error err) {
        LOG_ERROR("login failed! parse json err: %s", response.c_str());
        SyncCallback(doLoginServerParam);
        return;
    }
}

NAMESPACE_END(login)
NAMESPACE_END(z)