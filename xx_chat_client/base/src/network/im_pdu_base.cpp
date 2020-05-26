/**
 * Copyright 2018-2019, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: 消息结构及基本操作封装
 */

#include "utils.h"
#include "im_pdu_base.h"
//#include "im.base.pb.h"

ImPdu::ImPdu()
{
	pduHeader_.version = IM_PDU_VERSION;
	pduHeader_.flag = 0;
	//pduHeader_.service_id = im::base::SID_OTHER;
	pduHeader_.command_id = 0;
	pduHeader_.seq_num = 0;
    pduHeader_.reversed = 0;
}

uchar_t* ImPdu::GetBuffer()
{
    return buf_.GetBuffer();
}

uint32_t ImPdu::GetLength()
{
    return buf_.GetWriteOffset();
}

uchar_t* ImPdu::GetBodyData()
{
    return buf_.GetBuffer() + sizeof(PduHeader_t);
}

uint32_t ImPdu::GetBodyLength()
{
    uint32_t body_length = 0;
    body_length = buf_.GetWriteOffset() - sizeof(PduHeader_t);
    return body_length;
}

void ImPdu::WriteHeader()
{
	uchar_t* buf = GetBuffer();

	ByteStream::WriteInt32(buf, GetLength());
	ByteStream::WriteUint16(buf + 4, pduHeader_.version);
	ByteStream::WriteUint16(buf + 6, pduHeader_.flag);
	ByteStream::WriteUint16(buf + 8, pduHeader_.service_id);
	ByteStream::WriteUint16(buf + 10, pduHeader_.command_id);
    ByteStream::WriteUint16(buf + 12, pduHeader_.seq_num);
    ByteStream::WriteUint16(buf + 14, pduHeader_.reversed);
}

void ImPdu::SetVersion(uint16_t version)
{
	uchar_t* buf = GetBuffer();
	ByteStream::WriteUint16(buf + 4, version);
}

void ImPdu::SetFlag(uint16_t flag)
{
	uchar_t* buf = GetBuffer();
	ByteStream::WriteUint16(buf + 6, flag);
}

void ImPdu::SetServiceId(uint16_t service_id)
{
    uchar_t* buf = GetBuffer();
    ByteStream::WriteUint16(buf + 8, service_id);
}

void ImPdu::SetCommandId(uint16_t command_id)
{
    uchar_t* buf = GetBuffer();
    ByteStream::WriteUint16(buf + 10, command_id);
}

void ImPdu::SetError(uint16_t error)
{
    uchar_t* buf = GetBuffer();
    ByteStream::WriteUint16(buf + 12, error);
}

void ImPdu::SetSeqNum(uint16_t seq_num)
{
	uchar_t* buf = GetBuffer();
	ByteStream::WriteUint16(buf + 12, seq_num);
}

void ImPdu::SetReversed(uint32_t reversed)
{
    uchar_t* buf = GetBuffer();
    ByteStream::WriteUint16(buf+14, reversed);
}

int ImPdu::ReadPduHeader(uchar_t* buf, uint32_t len)
{
	int ret = -1;
	if (len >= IM_PDU_HEADER_LEN && buf) {
		ByteStream is(buf, len);
		is >> pduHeader_.length;
		is >> pduHeader_.version;
		is >> pduHeader_.flag;
		is >> pduHeader_.service_id;
		is >> pduHeader_.command_id;
		is >> pduHeader_.seq_num;
        is >> pduHeader_.reversed;
		ret = 0;
	}
	return ret;
}

std::shared_ptr<ImPdu> ImPdu::ReadPdu(uchar_t* buf, uint32_t len)
{
	uint32_t pdu_len = 0;
    if (!IsPduAvailable(buf, len, pdu_len)) {
        return nullptr;
    }

	uint16_t service_id = ByteStream::ReadUint16(buf + 8);
	uint16_t command_id = ByteStream::ReadUint16(buf + 10);

    std::shared_ptr<ImPdu> pPdu = std::make_shared<ImPdu>();
    pPdu->Write(buf, pdu_len);
    pPdu->ReadPduHeader(buf, IM_PDU_HEADER_LEN);
    return pPdu;
}

bool ImPdu::IsPduAvailable(uchar_t* buf, uint32_t len, uint32_t& pdu_len)
{
    if (len < IM_PDU_HEADER_LEN) {
        return false;
    }
	pdu_len = ByteStream::ReadUint32(buf);
	if (pdu_len > len) {
		//printf("pdu_len=%d, len=%d\n", pdu_len, len);
		return false;
	}
    if(0 == pdu_len) {
        throw PduException(1, "pdu_len is 0");
    }
	return true;
}

void ImPdu::SetPBMsg(const google::protobuf::MessageLite* msg)
{
    // 设置包体，则需要重置下空间
    buf_.Read(NULL, buf_.GetWriteOffset());
    buf_.Write(NULL, sizeof(PduHeader_t));
    uint32_t msg_size = msg->ByteSize();
    uchar_t* szData = new uchar_t[msg_size];
    // ALLOC_FAIL_ASSERT(szData)
    if (!msg->SerializeToArray(szData, msg_size)) {
        printf("pb msg miss required fields.\n");
    }
    buf_.Write(szData, msg_size);
    delete[] szData;
    WriteHeader();
}
