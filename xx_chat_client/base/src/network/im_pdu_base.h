/**
 * Copyright 2018-2019, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: 消息结构及基本操作封装
 */

#ifndef BASE_IM_PDU_BASE_H
#define BASE_IM_PDU_BASE_H

#include <memory>
#include "util_pdu.h"
#include "google/protobuf/message_lite.h"

#define IM_PDU_HEADER_LEN		16
#define IM_PDU_VERSION			1

#define ALLOC_FAIL_ASSERT(p) if (!p) {           \
    throw CPduException(m_pdu_header.service_id, \
        m_pdu_header.command_id,                 \
        ERROR_CODE_ALLOC_FAILED,                 \
        "allocate failed");                      \
}

#define CHECK_PB_PARSE_MSG(ret) {         \
    if (ret == false) {                   \
        printf("parse pb msg failed.\n"); \
        return;                           \
    }                                     \
}

#ifndef BUILD_STATIC
  #ifdef BUILD_PDU
    #define DLL_MODIFIER __declspec(dllexport)
  #else
    #define DLL_MODIFIER __declspec(dllimport)
  #endif
#else
  #define DLL_MODIFIER
#endif


typedef struct {
    uint32_t 	length;		  // the whole pdu length
    uint16_t 	version;	  // pdu version number
    uint16_t	flag;		  // not used
    uint16_t	service_id;	  //
    uint16_t	command_id;	  //
    uint16_t	seq_num;      // 包序号
    uint16_t    reversed;     // 保留
} PduHeader_t;

class DLL_MODIFIER ImPdu
{
public:
    ImPdu();
    virtual ~ImPdu() {}
    
    uchar_t* GetBuffer();
    uint32_t GetLength();
    uchar_t* GetBodyData();
    uint32_t GetBodyLength();
    
    uint16_t GetVersion() { return pduHeader_.version; }
    uint16_t GetFlag() { return pduHeader_.flag; }
    uint16_t GetServiceId() { return pduHeader_.service_id; }
    uint16_t GetCommandId() { return pduHeader_.command_id; }
    uint16_t GetSeqNum() { return pduHeader_.seq_num; }
    uint32_t GetReversed() { return pduHeader_.reversed; }
    
    void SetVersion(uint16_t version);
    void SetFlag(uint16_t flag);
    void SetServiceId(uint16_t service_id);
    void SetCommandId(uint16_t command_id);
    void SetError(uint16_t error);
    void SetSeqNum(uint16_t seq_num);
    void SetReversed(uint32_t reversed);
    void WriteHeader();
    
    static bool IsPduAvailable(uchar_t* buf, uint32_t len, uint32_t& pdu_len);
    static ImPdu* ReadPdu(uchar_t* buf, uint32_t len);
    void Write(uchar_t* buf, uint32_t len) { buf_.Write((void*)buf, len); }
    int ReadPduHeader(uchar_t* buf, uint32_t len);
    void SetPBMsg(const google::protobuf::MessageLite* msg);
    
protected:
    SimpleBuffer buf_;
    PduHeader_t	pduHeader_;
};

typedef std::shared_ptr<ImPdu> ImPduPtr;

#endif  // BASE_IM_PDU_BASE_H