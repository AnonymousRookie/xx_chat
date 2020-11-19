/**
* Copyright 2018-2020, AnonymousRookie. All rights reserved.
* https://github.com/AnonymousRookie/xx_chat
* Author: AnonymousRookie (357688981 at qq dot com)
* Description: 常见文件处理函数
*/

#ifndef BASE_SRC_FILE_UTIL_H
#define BASE_SRC_FILE_UTIL_H

#include <vector>
#include <sstream>
#include <QString>
#include <QFile>
#include "base_global.h"
#include "defines.h"

NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(utils)

// 计算文件MD5
BASE_EXPORT std::string getFileMd5(const std::string& sourceFilePath);
// 计算文件SHA1
BASE_EXPORT std::string getFileSha1(const std::string& sourceFilePath);
// 文件是否存在
BASE_EXPORT bool isFileExist(const std::string& filename);
// 是否是目录
BASE_EXPORT bool isPath(const std::string& path);
// 路径是否存在
BASE_EXPORT bool isPathExist(const std::string& path);
// 创建目录
BASE_EXPORT bool createDirIfNotExist(const QString& fullPath);
// 获取路径dirPath中的所有文件名
BASE_EXPORT bool getFilenameList(const std::string& dirPath, std::vector<std::string>* fileNameVector/*out*/);
// 根据绝对路径文件名获取文件名
BASE_EXPORT std::string getFileName(const std::string& absolutePath);
// 删除文件
BASE_EXPORT void deleteFile(const std::string& filename);
// 删除文件夹
BASE_EXPORT bool deleteDirectory(const QString& path);
// 判断文件夹是否为空
BASE_EXPORT bool isDirEmpty(const QString& path);

template <typename T>
inline void writeKeyValue(QFile* file, const std::string& key, const T& val)
{
    Z_CHECK(file);
    std::ostringstream oss("");
    oss.str("");
    oss << key << val << "\n";
    file->write(oss.str().c_str());
}

template <>
inline void writeKeyValue(QFile* file, const std::string& key, const std::vector<std::string>& vec)
{
    Z_CHECK(file);
    std::ostringstream oss("");
    oss.str("");
    oss << key;
    file->write(oss.str().c_str());

    Z_CHECK(!vec.empty());
    oss.str("");
    for (int i = 0; i < vec.size() - 1; ++i) {
        oss << vec[i] << ",";
    }
    oss << vec[vec.size()-1] << "\n";
    file->write(oss.str().c_str());
}

NAMESPACE_END(utils)
NAMESPACE_END(z)

#endif  // BASE_SRC_STRING_UTIL_H