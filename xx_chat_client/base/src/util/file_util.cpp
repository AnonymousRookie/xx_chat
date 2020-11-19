/**
* Copyright 2018-2020, AnonymousRookie. All rights reserved.
* https://github.com/AnonymousRookie/xx_chat
* Author: AnonymousRookie (357688981 at qq dot com)
* Description: 常见文件处理函数
*/

#include <sstream>
#include <QFile>
#include <QStringList>
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <QCryptographicHash>
#include "file_util.h"
#include "string_util.h"

NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(utils)

std::string getFileMd5(const std::string& sourceFilePath) 
{
    std::string md5("");

    QFile sourceFile(str2qstr(sourceFilePath));
    qint64 fileSize = sourceFile.size();
    const qint64 bufferSize = 10240;

    if (sourceFile.open(QIODevice::ReadOnly)) {
        char buffer[bufferSize] = { 0 };
        int bytesRead = 0;
        int readSize = qMin(fileSize, bufferSize);

        QCryptographicHash hash(QCryptographicHash::Md5);

        while (readSize > 0 && (bytesRead = sourceFile.read(buffer, readSize)) > 0) {
            fileSize -= bytesRead;
            hash.addData(buffer, bytesRead);
            readSize = qMin(fileSize, bufferSize);
        }
        sourceFile.close();
        md5 = qStr2Str(QString(hash.result().toHex()));
        return md5;
    }
    return md5;
}

std::string getFileSha1(const std::string& sourceFilePath)
{
    std::string sha1("");

    QFile sourceFile(str2qstr(sourceFilePath));
    qint64 fileSize = sourceFile.size();
    const qint64 bufferSize = 10240;

    if (sourceFile.open(QIODevice::ReadOnly)) {
        char buffer[bufferSize] = { 0 };
        int bytesRead = 0;
        int readSize = qMin(fileSize, bufferSize);

        QCryptographicHash hash(QCryptographicHash::Sha1);

        while (readSize > 0 && (bytesRead = sourceFile.read(buffer, readSize)) > 0) {
            fileSize -= bytesRead;
            hash.addData(buffer, bytesRead);
            readSize = qMin(fileSize, bufferSize);
        }
        sourceFile.close();
        sha1 = qStr2Str(QString(hash.result().toHex()));
        return sha1;
    }
    return sha1;
}

bool isFileExist(const std::string& filename)
{
    QString qsFilepath = QString::fromLocal8Bit(filename.c_str());
    QFileInfo fileInfo(qsFilepath);
    return fileInfo.isFile();
}

bool isPath(const std::string& path)
{
    return (path.size() >= 3) 
        && ((path[0] >= 'a' && path[0] <= 'z') || (path[0] >= 'A' && path[0] <= 'Z'))
        && (path[1] == ':')
        && (path[2] == '\\' || path[2] == '/');
}

bool isPathExist(const std::string& path)
{
    QString qsPath = QString::fromLocal8Bit(path.c_str());
    QDir dir(qsPath);
    return dir.exists();
}

bool createDirIfNotExist(const QString& fullPath)
{
    QDir dir(fullPath);
    if (dir.exists()) {
        return true;
    }
    return dir.mkpath(fullPath);  // 可创建多级目录
}

bool getFilenameList(const std::string& dirPath, std::vector<std::string>* fileNameVector)
{
    QDir dir(z::utils::str2qstr(dirPath));
    if(!dir.exists()) {
        return false;
    }
    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    QFileInfoList list = dir.entryInfoList();
    int fileNum = list.count();
    if(fileNum <= 0) {
        return false;
    }
    fileNameVector->clear();
    for(int i=0; i < fileNum; ++i) {
        QFileInfo fileInfo = list.at(i);
        const auto& fileName = fileInfo.absoluteFilePath();
        fileNameVector->push_back(z::utils::qStr2Str(fileName));
    }
    return true;
}

std::string getFileName(const std::string& absolutePath)
{
    QFileInfo fileinfo(z::utils::str2qstr(absolutePath));
    std::string filename = qStr2Str(fileinfo.fileName());
    return filename;
}

void deleteFile(const std::string& filename)
{
    bool bZipExist = z::utils::isFileExist(filename);
    if (bZipExist) {
        remove(filename.c_str());
    }
}

bool deleteDirectory(const QString& path)
{
    if (path.isEmpty()) {
        return false;
    }
    QDir dir(path);
    if(!dir.exists()) {
        return true;
    }
    dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
    QFileInfoList fileList = dir.entryInfoList();
    foreach (QFileInfo fi, fileList) 
    {
        if (fi.isFile()) {
            fi.dir().remove(fi.fileName());
        } else {
            deleteDirectory(fi.absoluteFilePath());
        }
    }
    return dir.rmpath(dir.absolutePath());
}

bool isDirEmpty(const QString& path)
{
    if (path.isEmpty()) {
        return true;
    }
    QDir dir(path);
    if (!dir.exists()) {
        return true;
    }
    dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
    QFileInfoList fileList = dir.entryInfoList();
    return fileList.empty();
}

NAMESPACE_END(utils)
NAMESPACE_END(z)