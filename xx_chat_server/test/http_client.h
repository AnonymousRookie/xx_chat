/**
 * Copyright 2018-2019, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: HttpClient
 */

#ifndef TEST_CLIENT_HTTP_CLIENT_H
#define TEST_CLIENT_HTTP_CLIENT_H

#include <string>
#include <curl/curl.h>

class HttpClient
{
public:
    HttpClient();
    ~HttpClient();

public:
    CURLcode Get(const std::string& url, std::string* response);
    CURLcode Post(const std::string& url, const std::string& post, std::string* response);
};


#endif  // TEST_CLIENT_HTTP_CLIENT_H