/**
* Copyright 2018-2020, AnonymousRookie. All rights reserved.
* https://github.com/AnonymousRookie/xx_chat
* Author: AnonymousRookie (357688981 at qq dot com)
* Description: �����ļ�������
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

// �����ļ�MD5
BASE_EXPORT std::string getFileMd5(const std::string& sourceFilePath);
// �����ļ�SHA1
BASE_EXPORT std::string getFileSha1(const std::string& sourceFilePath);
// �ļ��Ƿ����
BASE_EXPORT bool isFileExist(const std::string& filename);
// �Ƿ���Ŀ¼
BASE_EXPORT bool isPath(const std::string& path);
// ·���Ƿ����
BASE_EXPORT bool isPathExist(const std::string& path);
// ����Ŀ¼
BASE_EXPORT bool createDirIfNotExist(const QString& fullPath);
// ��ȡ·��dirPath�е������ļ���
BASE_EXPORT bool getFilenameList(const std::string& dirPath, std::vector<std::string>* fileNameVector/*out*/);
// ���ݾ���·���ļ�����ȡ�ļ���
BASE_EXPORT std::string getFileName(const std::string& absolutePath);
// ɾ���ļ�
BASE_EXPORT void deleteFile(const std::string& filename);
// ɾ���ļ���
BASE_EXPORT bool deleteDirectory(const QString& path);
// �ж��ļ����Ƿ�Ϊ��
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