/**
 * Copyright 2018-2019, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: defines
 */

#ifndef BASE_DEFINES_H
#define BASE_DEFINES_H


#define Z_CHECK(p) if (!(p)) return;
#define Z_CHECK_RETURN(p, r) if (!(p)) return r;
#define Z_CHECK_CONTINUE(p) if (!(p)) continue;
#define Z_CHECK_BREAK(p) if (!(p)) break;


#define NAMESPACE_BEGIN(n) namespace n {
#define NAMESPACE_END(n)   }


enum {
    USER_CNT_INC = 1,
    USER_CNT_DEC = 2,
};


typedef struct {
    uint32_t user_id;
    uint32_t conn_cnt;
} user_conn_t;


typedef struct {
    uint32_t user_id;
    uint32_t status;
} user_stat_t;


#endif  // BASE_DEFINES_H