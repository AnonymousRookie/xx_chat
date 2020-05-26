/**
 * Copyright 2019-2020, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: NONE
 */

#include "../stdafx.h"
#include <sstream>
#include "../proxy_conn.h"
#include "../db_pool.h"
#include "user_model.h"


NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(business)

UserModel* UserModel::instance_ = NULL;

UserModel::UserModel()
{

}

UserModel::~UserModel()
{

}

UserModel* UserModel::GetInstance()
{
    if (!instance_) {
        instance_ = new UserModel();
    }
    return instance_;
}

void UserModel::GetUserInfos(std::list<im::base::UserInfo>& userInfoList)
{
    DBManager* dbManager = DBManager::GetInstance();
    DBConn* dbConn = dbManager->GetDBConn("slave");
    Z_CHECK(dbConn);

    std::ostringstream oss("");
    oss.str("");
    oss << "select * from t_user";
    ResultSet* resultSet = dbConn->ExecuteQuery(oss.str().c_str());

    if (resultSet) {
        while (resultSet->Next()) {
            im::base::UserInfo userInfo;
            userInfo.set_user_id(resultSet->GetInt("id"));
            userInfo.set_user_name(resultSet->GetString("name"));
            userInfoList.push_back(userInfo);
        }
    }

    dbManager->ReleaseDBConn(dbConn);
}


NAMESPACE_END(business)
NAMESPACE_END(z)
