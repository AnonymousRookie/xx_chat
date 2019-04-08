/**
 * Copyright 2018-2019, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: 消息缓冲区定义及序列化反序列化操作封装
 */

#ifndef BASE_UTIL_PDU_H
#define BASE_UTIL_PDU_H

#include <set>
#include <map>
#include <list>
#include <string>
#include "types.h"

#ifdef WIN32
  #ifdef BUILD_PDU
    #define DLL_MODIFIER __declspec(dllexport)
  #else
    #define DLL_MODIFIER __declspec(dllimport)
  #endif
#else
  #define DLL_MODIFIER
#endif

// exception code
#define ERROR_CODE_PARSE_FAILED 		1
#define ERROR_CODE_WRONG_SERVICE_ID		2
#define ERROR_CODE_WRONG_COMMAND_ID		3
#define ERROR_CODE_ALLOC_FAILED			4

class PduException {
public:
	PduException(uint32_t service_id, uint32_t command_id, uint32_t error_code, const char* error_msg)
	{
		serviceId_ = service_id;
		commandId_ = command_id;
		errorCode_ = error_code;
		errorMsg_ = error_msg;
	}
	PduException(uint32_t error_code, const char* error_msg)
	{
		serviceId_ = 0;
		commandId_ = 0;
		errorCode_ = error_code;
		errorMsg_ = error_msg;
	}

	virtual ~PduException() {}

	uint32_t GetServiceId() { return serviceId_; }
	uint32_t GetCommandId() { return commandId_; }
	uint32_t GetErrorCode() { return errorCode_; }
	char* GetErrorMsg() { return (char*)errorMsg_.c_str(); }

private:
	uint32_t	serviceId_;
	uint32_t	commandId_;
	uint32_t	errorCode_;
	std::string errorMsg_;
};

class DLL_MODIFIER SimpleBuffer
{
public:
	SimpleBuffer();
	~SimpleBuffer();

	uchar_t* GetBuffer() { return buffer_; }
	uint32_t GetAllocSize() { return allocSize_; }
	uint32_t GetWriteOffset() { return writeOffset_; }
	void IncWriteOffset(uint32_t len) { writeOffset_ += len; }

	void Extend(uint32_t len);
	uint32_t Write(void* buf, uint32_t len);
	uint32_t Read(void* buf, uint32_t len);

private:
	uchar_t* buffer_;
	uint32_t allocSize_;
	uint32_t writeOffset_;
};

class ByteStream
{
public:
	ByteStream(uchar_t* buf, uint32_t len);
	ByteStream(SimpleBuffer* pSimpBuf, uint32_t pos);
	~ByteStream() {}

	unsigned char* GetBuf() { 
        return simpleBuf_ ? simpleBuf_->GetBuffer() : buf_; 
    }
	uint32_t GetPos() { return pos_; }
	uint32_t GetLen() { return len_; }

	void Skip(uint32_t len) {
		pos_ += len;
		if(pos_ > len_) {
			throw PduException(ERROR_CODE_PARSE_FAILED, "parase packet failed!");
		}
	}

	static int16_t ReadInt16(uchar_t* buf);
	static uint16_t ReadUint16(uchar_t* buf);
	static int32_t ReadInt32(uchar_t* buf);
	static uint32_t ReadUint32(uchar_t* buf);
	static void WriteInt16(uchar_t* buf, int16_t data);
	static void WriteUint16(uchar_t* buf, uint16_t data);
	static void WriteInt32(uchar_t* buf, int32_t data);
	static void WriteUint32(uchar_t* buf, uint32_t data);

	void operator<< (int8_t data);
	void operator<< (uint8_t data);
	void operator<< (int16_t data);
	void operator<< (uint16_t data);
	void operator<< (int32_t data);
	void operator<< (uint32_t data);

	void operator>> (int8_t& data);
	void operator>> (uint8_t& data);
	void operator>> (int16_t& data);
	void operator>> (uint16_t& data);
	void operator>> (int32_t& data);
	void operator>> (uint32_t& data);

	void WriteString(const char* str);
	void WriteString(const char* str, uint32_t len);
	char* ReadString(uint32_t& len);

	void WriteData(uchar_t* data, uint32_t len);
	uchar_t* ReadData(uint32_t& len);

private:
	void WriteByte(void* buf, uint32_t len);
	void ReadByte(void* buf, uint32_t len);

private:
	SimpleBuffer*	simpleBuf_;
	uchar_t*		buf_;
	uint32_t		len_;
	uint32_t		pos_;
};

#endif  // BASE_UTIL_PDU_H