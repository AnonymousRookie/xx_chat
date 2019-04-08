/**
 * Copyright 2018-2019, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: DBPool
 */

#include "stdafx.h"
#include "config_file_reader.h"
#include "db_pool.h"

#define MIN_DB_CONN_CNT 2

DBManager* DBManager::dbManager_ = NULL;

ResultSet::ResultSet(MYSQL_RES* res)
{
    res_ = res;
    // map table field key to index in the result array
    int num_fields = mysql_num_fields(res);
    MYSQL_FIELD* fields = mysql_fetch_fields(res);
    for (int i = 0; i < num_fields; ++i) {
        keyMap_.insert(std::make_pair(fields[i].name, i));
    }
}

ResultSet::~ResultSet()
{
}

bool ResultSet::Next()
{
    row_ = mysql_fetch_row(res_);
    if (row_) {
        return true;
    } else {
        return false;
    }
}

int ResultSet::GetInt(const char* key)
{
    int index = GetIndex(key);
    if (index == -1) {
        return 0;
    } else {
        return atoi(row_[index]);
    }
}

char* ResultSet::GetString(const char * key)
{
    int index = GetIndex(key);
    if (index == -1) {
        return nullptr;
    } else {
        return row_[index];
    }
}

int ResultSet::GetIndex(const char* key)
{
    auto it = keyMap_.find(key);
    if (it != keyMap_.end()) {
        return it->second;
    }
    return -1;
}


DBConn::DBConn(DBPool* pool)
{
    dbPool_ = pool;
    mysql_ = NULL;
}

DBConn::~DBConn()
{

}

int DBConn::Init()
{
    mysql_ = mysql_init(NULL);
    if (!mysql_) {
        LOG_INFO("mysql_init failed!");
        return 1;
    }

    my_bool reconnect = true;

    mysql_options(mysql_, MYSQL_OPT_RECONNECT, &reconnect);
    mysql_options(mysql_, MYSQL_SET_CHARSET_NAME, "utf8mb4");

    auto ret = mysql_real_connect(mysql_, 
        dbPool_->GetDBServerIP(), 
        dbPool_->GetUsername(),
        dbPool_->GetPasswrod(),
        dbPool_->GetDBName(),
        dbPool_->GetDBServerPort(),
        NULL,
        0);

    if (!ret) {
        LOG_INFO("mysql_real_connect failed: %s", mysql_error(mysql_));
        return 2;
    }

    return 0;
}

ResultSet* DBConn::ExecuteQuery(const char* sqlQuery)
{
    mysql_ping(mysql_);
    if (mysql_real_query(mysql_, sqlQuery, strlen(sqlQuery))) {
        LOG_INFO("mysql_real_query failed: %s, sql: %s", mysql_error(mysql_), sqlQuery);
        return NULL;
    }
    MYSQL_RES* res = mysql_store_result(mysql_);
    if (!res) {
        LOG_INFO("mysql_store_result failed: %s", mysql_error(mysql_));
        return NULL;
    }
    ResultSet* resultSet = new ResultSet(res);
    return resultSet;
}

bool DBConn::ExecuteUpdate(const char* sqlQuery)
{
    mysql_ping(mysql_);

    if (mysql_real_query(mysql_, sqlQuery, strlen(sqlQuery))) {
        LOG_INFO("mysql_real_query failed: %s, sql: %s", mysql_error(mysql_), sqlQuery);
        return false;
    }
    if (mysql_affected_rows(mysql_) > 0) {
        return true;
    } 
    return false;
}

uint32_t DBConn::GetInsertId()
{
    return uint32_t();
}

const char* DBConn::GetPoolName()
{
    return dbPool_->GetPoolName();
}

DBPool::DBPool(const std::string& pool_name,
    const std::string& db_server_ip,
    uint16_t db_server_port,
    const std::string& username,
    const std::string& password,
    const std::string& db_name,
    uint32_t max_conn_cnt)
{
    poolName_ = pool_name;
    dbServerIp_ = db_server_ip;
    dbServerPort_ = db_server_port;
    userName_ = username;
    password_ = password;
    dbName_ = db_name;
    curDbConnCnt_ = MIN_DB_CONN_CNT;
    maxDbConnCnt_ = max_conn_cnt;
}

DBPool::~DBPool()
{
    for (auto it = freeList_.begin(); it != freeList_.end(); ++it) {
        DBConn* conn = *it;
        delete conn;
    }
    freeList_.clear();
}

int DBPool::Init()
{
    for (int i = 0; i < curDbConnCnt_; ++i) {
        DBConn* conn = new DBConn(this);
        int ret = conn->Init();
        if (ret) {
            delete conn;
            return ret;
        }
        freeList_.push_back(conn);
    }
    LOG_INFO("db pool: %s, size: %d", poolName_.c_str(), (int)freeList_.size());
    return 0;
}

DBConn* DBPool::GetDBConn()
{
    std::unique_lock<std::mutex> lock(mutex_);

    while (freeList_.empty()) {
        if (curDbConnCnt_ >= maxDbConnCnt_) {
            notEmpty_.wait(lock);
        } else {
            DBConn* conn = new DBConn(this);
            if (conn->Init()) {
                LOG_ERROR("DBConn init failed!");
                delete conn;
                return NULL;
            } else {
                freeList_.push_back(conn);
                ++curDbConnCnt_;
                LOG_INFO("new db conn: %s, conn_count: %d", poolName_.c_str(), curDbConnCnt_);
            }

        }
    }
    DBConn* conn = freeList_.front();
    freeList_.pop_front();

    return conn;
}

void DBPool::ReleaseDBConn(DBConn* conn)
{
    std::unique_lock<std::mutex> lock(mutex_);

    auto it = freeList_.begin();
    for (; it != freeList_.end(); ++it) {
        if (*it == conn) {
            break;
        }
    }
    if (it == freeList_.end()) {
        freeList_.push_back(conn);
    }
    notEmpty_.notify_one();
}

DBManager::DBManager()
{

}

DBManager::~DBManager()
{
}

DBManager* DBManager::GetInstance()
{
    if (!dbManager_) {
        dbManager_ = new DBManager();
        if (dbManager_->Init()) {
            delete dbManager_;
            dbManager_ = NULL;
        }
    }
    return dbManager_;
}

int DBManager::Init()
{
    ConfigFileReader configFileReader("db_proxy_server.json");
    auto& dom = configFileReader.GetDom();

    auto& mysql_conf = dom["mysql_conf"];
    const std::string& db_instances = mysql_conf["db_instances"].GetString();

    const auto& v = z::utils::Strtok(db_instances, ",");
    for (int i = 0; i < v.size(); ++i) {
        std::string poolname = v[i];
        auto& db_instance = mysql_conf[v[i].c_str()];
        std::string host = db_instance["host"].GetString();
        uint16_t port = db_instance["port"].GetUint();
        std::string db_name = db_instance["db_name"].GetString();
        std::string username = db_instance["username"].GetString();
        std::string password = db_instance["password"].GetString();
        uint32_t max_conn = db_instance["max_conn"].GetUint();

        DBPool* pool = new DBPool(poolname, host, port, username, password, db_name, max_conn);
        if (pool->Init()) {
            LOG_ERROR("init db instance failed: %s", poolname.c_str());
            return 3;
        }
        dbPollMap_.insert(std::make_pair(poolname, pool));
    }
    return 0;
}

DBConn* DBManager::GetDBConn(const char* dbPoolName)
{
    auto it = dbPollMap_.find(dbPoolName);
    if (it != dbPollMap_.end()) {
        DBPool* dbPool = it->second;
        return dbPool->GetDBConn();
    }
    return nullptr;
}

void DBManager::ReleaseDBConn(DBConn* conn)
{
    if (!conn) {
        return;
    }
    auto it = dbPollMap_.find(conn->GetPoolName());
    if (it != dbPollMap_.end()) {
        DBPool* dbPool = it->second;
        dbPool->ReleaseDBConn(conn);
    }
}