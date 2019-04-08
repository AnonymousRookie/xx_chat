/**
 * Copyright 2018-2019, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: 登录验证
 */

#include "stdafx.h"
#include <sstream>
#include "utils.h"
#include "defines.h"
#include "inter_login.h"
#include "../db_pool.h"

bool InterLoginStrategy::DoLogin(const std::string& name, 
                                 const std::string& password, 
                                 im::base::UserInfo& user)
{
    bool ret = false;
    std::ostringstream oss("");

    DBManager* dbManager = DBManager::GetInstance();
    DBConn* dbConn = dbManager->GetDBConn("slave");
    Z_CHECK_RETURN(dbConn, ret);
    
    oss.str("");
    oss << "select * from t_user where name='"
        << name
        << "'";
    ResultSet* resultSet = dbConn->ExecuteQuery(oss.str().c_str());
    if (resultSet) {
        std::string resultPasswd("");
        uint32_t resultId = 0;
        while (resultSet->Next()) {
            resultId = resultSet->GetInt("id");
            resultPasswd = resultSet->GetString("password");
            break;
        }
        if (password == resultPasswd) {
            ret = true;
            user.set_user_id(resultId);
            user.set_user_name(name);
        }
        delete resultSet;
    }

    dbManager->ReleaseDBConn(dbConn);

    return ret;
}
