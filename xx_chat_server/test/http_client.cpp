/**
 * Copyright 2018-2019, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: HttpClient
 */

#include "stdafx.h"
#include "http_client.h"

static size_t OnWriteData(void* buffer, size_t size, size_t nmemb, void* lpVoid)
{
    std::string* str = dynamic_cast<std::string*>((std::string*)lpVoid);
    if (NULL == str || NULL == buffer) {
        return -1;
    }
    char* pData = (char*)buffer;
    str->append(pData, size * nmemb);
    return nmemb;
}

HttpClient::HttpClient()
{

}

HttpClient::~HttpClient()
{
    //printf("HttpClient::~HttpClient()\n");
}

CURLcode HttpClient::Get(const std::string& url, std::string* response)
{
    CURLcode res;
    CURL* curl = curl_easy_init();
    if (!curl) {
        return CURLE_FAILED_INIT;
    }
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
    curl_easy_setopt(curl, CURLOPT_FORBID_REUSE, 1);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)response);
    /**
     * 当多个线程都使用超时处理的时候，同时主线程中有sleep或是wait等操作。
     * 如果不设置这个选项，libcurl将会发信号打断这个wait从而导致程序退出。
     */
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3);
    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    return res;
}

CURLcode HttpClient::Post(const std::string& url, const std::string& post, std::string* response)
{
    CURLcode res;
    CURL* curl = curl_easy_init();
    if (NULL == curl) {
        return CURLE_FAILED_INIT;
    }
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_FORBID_REUSE, 1);
    curl_easy_setopt(curl, CURLOPT_POST, 1);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post.c_str());
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)response);
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3);
    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    return res;
}
