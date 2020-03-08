/**
 * Copyright 2019-2020, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: 借助CURL实现HTTP的GET和POST操作
 */

#pragma once

#include <string>
#include <curl\curl.h>
#include "base\src\defines.h"
#include "base\src\types.h"

NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(core)

class HttpClient
{
public:
    HttpClient();
    ~HttpClient();

public:
    CURLcode Get(const std::string& url, std::string* response);
    CURLcode Post(const std::string& url, const std::string& post, std::string* response);
};

NAMESPACE_END(core)
NAMESPACE_END(z)
