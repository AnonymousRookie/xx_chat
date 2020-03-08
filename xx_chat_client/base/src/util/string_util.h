/**
* Copyright 2018-2019, AnonymousRookie. All rights reserved.
* https://github.com/AnonymousRookie/xx_chat
* Author: AnonymousRookie (357688981 at qq dot com)
* Description: �����ַ���������
*/

#ifndef BASE_SRC_STRING_UTIL_H
#define BASE_SRC_STRING_UTIL_H

#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <QString>
#include "../base_global.h"
#include "../defines.h"

NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(utils)

template <typename T>
inline T stringToNumber(const std::string& str)
{
    std::stringstream ss;
    T n = 0;
    ss << str;
    ss >> n;
    return n;
}

template <typename T>
std::string numberToString(T num)
{
    std::ostringstream oss("");
    oss << num;
    return oss.str();
}

// ���ַ���ת��ΪСд
inline std::string strToLower(const std::string& s)
{
    std::string res(s);
    std::transform(res.begin(), res.end(), res.begin(), ::tolower);
    return res;
}

// ���ַ���ת��Ϊ��д
inline std::string strToUpper(const std::string& s)
{
    std::string res(s);
    std::transform(res.begin(), res.end(), res.begin(), ::toupper);
    return res;
}

// ���ַ���str�е��Ӵ�strsrc�滻��strdst
inline void strReplace(std::string& str, const std::string& strsrc, const std::string& strdst)
{
    std::string::size_type pos = 0;
    auto srclen = strsrc.size();
    auto dstlen = strdst.size();
    while ((pos = str.find(strsrc, pos)) != std::string::npos) {
        str.replace(pos, srclen, strdst);
        pos += dstlen;
    }
}

inline bool charIsSpace(char c)
{
    return (c == '\t' || c == '\n' || c == ' ' || c == '\r');
}


// ȥ���ַ������˵Ŀո�
BASE_EXPORT std::string trimWhiteSpace(const std::string& str);
BASE_EXPORT std::string trimWhiteSpace(std::string* str);

// �ַ���str�Ƿ����ǰ׺prefix
inline bool startWithPrefixString(const std::string& str, const std::string& prefix)
{
    return str.size() >= prefix.size() &&
        str.compare(0, prefix.size(), prefix) == 0;
}

// �ַ���str�Ƿ���ں�׺suffix
inline bool endWithSuffixString(const std::string& str, const std::string& suffix)
{
    return str.size() >= suffix.size() &&
        str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

// �ж�array���Ƿ����element
inline bool findElement(const std::vector<std::string>& array, const std::string& element)
{
    return std::find(array.begin(), array.end(), element) != array.end();
}

inline bool findElement(const std::string& array, const std::string& element)
{
    return array.find(element) != std::string::npos;
}

// ʮ����תʮ������
inline std::string convertNumToHex(int num)
{
    char hex[32] = {0};
    sprintf_s(hex, sizeof(hex), "0x%x", num);
    return hex;
}

inline QString str2qstr(const std::string& str)
{
    return QString::fromLocal8Bit(str.data());
}

inline std::string qStr2Str(const QString& qstr)
{
    QByteArray cdata = qstr.toLocal8Bit();
    return std::string(cdata);
}

NAMESPACE_END(utils)
NAMESPACE_END(z)

#endif  // BASE_SRC_STRING_UTIL_H