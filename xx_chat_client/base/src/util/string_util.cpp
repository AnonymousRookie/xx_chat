/**
* Copyright 2018-2019, AnonymousRookie. All rights reserved.
* https://github.com/AnonymousRookie/xx_chat
* Author: AnonymousRookie (357688981 at qq dot com)
* Description: 常见字符串处理函数
*/

#include <sstream>
#include <iomanip>
#include "string_util.h"
#include "logger.h"

NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(utils)

std::string trimWhiteSpace(std::string* str)
{
    if (str == NULL) {
        return "";
    }
    int start = 0;
    int end = 0;
    for (start = 0; start < str->size(); ++start) {
        if (!charIsSpace(str->at(start))) {
            break;
        }
    }
    for (end = str->size() - 1; end >= 0; --end) {
        if (!charIsSpace(str->at(end))) {
            break;
        }
    }
    auto ret = str->substr(start, end - start + 1);
    *str = ret;
    return ret;
}

std::string trimWhiteSpace(const std::string& str)
{
    if (str.empty()) {
        return "";
    }
    int start = 0;
    int end = 0;
    for (start = 0; start < str.size(); ++start) {
        if (!charIsSpace(str.at(start))) {
            break;
        }
    }
    for (end = str.size() - 1; end >= 0; --end) {
        if (!charIsSpace(str.at(end))) {
            break;
        }
    }
    return str.substr(start, end - start + 1);
}

std::string trimRight(std::string* str, char ch)
{
    if (str == NULL) {
        return "";
    }
    int start = 0;
    int end = 0;
    for (end = str->size() - 1; end >= 0; --end) {
        if (str->at(end) != ch) {
            break;
        }
    }
    auto ret = str->substr(start, end-start + 1);
    *str = ret;
    return ret;
}

NAMESPACE_END(utils)
NAMESPACE_END(z)