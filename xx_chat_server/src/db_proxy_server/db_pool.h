/**
 * Copyright 2018-2019, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: DBPool
 */


#ifndef DB_PROXY_SERVER_DB_POOL_H
#define DB_PROXY_SERVER_DB_POOL_H

#include <string>
#include <map>
#include <list>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <mysql/mysql.h>
#include "utils.h"

class ResultSet
{
public:
    ResultSet(MYSQL_RES* res);
    virtual ~ResultSet();

    bool Next();
    int GetInt(const char* key);
    char* GetString(const char* key);

private:
    int GetIndex(const char* key);

private:
    MYSQL_RES* res_;
    MYSQL_ROW  row_;
    std::map<std::string, int> keyMap_;
};

class DBPool;
class DBConn
{
public:
    DBConn(DBPool* pool);
    virtual ~DBConn();

    int Init();

    ResultSet* ExecuteQuery(const char* sqlQuery);
    bool ExecuteUpdate(const char* sqlQuery);

    uint32_t GetInsertId();
    const char* GetPoolName();
    MYSQL* GetMysql() { return mysql_; }

private:
    DBPool* dbPool_;  // to get MySql server information
    MYSQL* mysql_;
};

class DBPool
{
public:
    DBPool(const std::string& pool_name, 
           const std::string& db_server_ip,
           uint16_t db_server_port, 
           const std::string& username,
           const std::string& password,
           const std::string& db_name,
           uint32_t max_conn_cnt);

    ~DBPool();

    int Init();
    DBConn* GetDBConn();
    void ReleaseDBConn(DBConn* conn);

    const char* GetPoolName() { return poolName_.c_str(); }
    const char* GetDBServerIP() { return dbServerIp_.c_str(); }
    uint16_t GetDBServerPort() { return dbServerPort_; }
    const char* GetUsername() { return userName_.c_str(); }
    const char* GetPasswrod() { return password_.c_str(); }
    const char* GetDBName() { return dbName_.c_str(); }

private:
    std::string poolName_;
    std::string dbServerIp_;
    uint16_t dbServerPort_;
    std::string userName_;
    std::string password_;
    std::string dbName_;
    uint32_t curDbConnCnt_;
    uint32_t maxDbConnCnt_;

    std::list<DBConn*> freeList_;

    std::mutex mutex_;
    std::condition_variable notEmpty_;
};

// manage db pool (master for write and slave for read)
class DBManager
{
public:
    virtual ~DBManager();
    static DBManager* GetInstance();

    int Init();
    DBConn* GetDBConn(const char* dbPoolName);
    void ReleaseDBConn(DBConn* conn);

private:
    DBManager();

private:
    static DBManager* dbManager_;
    std::map<std::string, DBPool*> dbPollMap_;
};

#endif  // DB_PROXY_SERVER_DB_POOL_H