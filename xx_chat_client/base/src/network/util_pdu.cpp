/**
 * Copyright 2018-2019, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: 消息缓冲区定义及序列化反序列化操作封装
 */

#include <stdlib.h>
#include <string.h>
#include "util_pdu.h"


SimpleBuffer::SimpleBuffer()
{
    buffer_ = NULL;
    allocSize_ = 0;
    writeOffset_ = 0;
}

SimpleBuffer::~SimpleBuffer()
{
    allocSize_ = 0;
    writeOffset_ = 0;
    if (buffer_) {
        free(buffer_);
        buffer_ = NULL;
    }
}

void SimpleBuffer::Extend(uint32_t len)
{
    allocSize_ = writeOffset_ + len;
    allocSize_ += allocSize_ >> 2;    // increase by 1/4 allocate size
    uchar_t* new_buf = (uchar_t*)realloc(buffer_, allocSize_);
    buffer_ = new_buf;
}

uint32_t SimpleBuffer::Write(void* buf, uint32_t len)
{
    if (writeOffset_ + len > allocSize_) {
        Extend(len);
    }
    if (buf) {
        memcpy(buffer_ + writeOffset_, buf, len);
    }
    writeOffset_ += len;
    return len;
}

uint32_t SimpleBuffer::Read(void* buf, uint32_t len)
{
    if (len > writeOffset_) {
        len = writeOffset_;
    }

    if (buf) {
        memcpy(buf, buffer_, len);
    }

    writeOffset_ -= len;
    memmove(buffer_, buffer_ + len, writeOffset_);
    return len;
}

ByteStream::ByteStream(uchar_t* buf, uint32_t len)
{
    buf_ = buf;
    len_ = len;
    simpleBuf_ = NULL;
    pos_ = 0;
}

ByteStream::ByteStream(SimpleBuffer* simpBuf, uint32_t pos)
{
    simpleBuf_ = simpBuf;
    pos_ = pos;
    buf_ = NULL;
    len_ = 0;
}

int16_t ByteStream::ReadInt16(uchar_t *buf)
{
    int16_t data = (buf[0] << 8) | buf[1];
    return data;
}

uint16_t ByteStream::ReadUint16(uchar_t* buf)
{
    uint16_t data = (buf[0] << 8) | buf[1];
    return data;
}

int32_t ByteStream::ReadInt32(uchar_t *buf)
{
    int32_t data = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
    return data;
}

uint32_t ByteStream::ReadUint32(uchar_t *buf)
{
    uint32_t data = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
    return data;
}

void ByteStream::WriteInt16(uchar_t *buf, int16_t data)
{
    buf[0] = static_cast<uchar_t>(data >> 8);
    buf[1] = static_cast<uchar_t>(data & 0xFF);
}

void ByteStream::WriteUint16(uchar_t *buf, uint16_t data)
{
    buf[0] = static_cast<uchar_t>(data >> 8);
    buf[1] = static_cast<uchar_t>(data & 0xFF);
}

void ByteStream::WriteInt32(uchar_t *buf, int32_t data)
{
    buf[0] = static_cast<uchar_t>(data >> 24);
    buf[1] = static_cast<uchar_t>((data >> 16) & 0xFF);
    buf[2] = static_cast<uchar_t>((data >> 8) & 0xFF);
    buf[3] = static_cast<uchar_t>(data & 0xFF);
}

void ByteStream::WriteUint32(uchar_t *buf, uint32_t data)
{
    buf[0] = static_cast<uchar_t>(data >> 24);
    buf[1] = static_cast<uchar_t>((data >> 16) & 0xFF);
    buf[2] = static_cast<uchar_t>((data >> 8) & 0xFF);
    buf[3] = static_cast<uchar_t>(data & 0xFF);
}

void ByteStream::operator << (int8_t data)
{
    WriteByte(&data, 1);
}

void ByteStream::operator << (uint8_t data)
{
    WriteByte(&data, 1);
}

void ByteStream::operator << (int16_t data)
{
    unsigned char buf[2];
    buf[0] = static_cast<uchar_t>(data >> 8);
    buf[1] = static_cast<uchar_t>(data & 0xFF);
    WriteByte(buf, 2);
}

void ByteStream::operator << (uint16_t data)
{
    unsigned char buf[2];
    buf[0] = static_cast<uchar_t>(data >> 8);
    buf[1] = static_cast<uchar_t>(data & 0xFF);
    WriteByte(buf, 2);
}

void ByteStream::operator << (int32_t data)
{
    unsigned char buf[4];
    buf[0] = static_cast<uchar_t>(data >> 24);
    buf[1] = static_cast<uchar_t>((data >> 16) & 0xFF);
    buf[2] = static_cast<uchar_t>((data >> 8) & 0xFF);
    buf[3] = static_cast<uchar_t>(data & 0xFF);
    WriteByte(buf, 4);
}

void ByteStream::operator << (uint32_t data)
{
    unsigned char buf[4];
    buf[0] = static_cast<uchar_t>(data >> 24);
    buf[1] = static_cast<uchar_t>((data >> 16) & 0xFF);
    buf[2] = static_cast<uchar_t>((data >> 8) & 0xFF);
    buf[3] = static_cast<uchar_t>(data & 0xFF);
    WriteByte(buf, 4);
}

void ByteStream::operator >> (int8_t& data)
{
    ReadByte(&data, 1);
}

void ByteStream::operator >> (uint8_t& data)
{
    ReadByte(&data, 1);
}

void ByteStream::operator >> (int16_t& data)
{
    unsigned char buf[2];

    ReadByte(buf, 2);

    data = (buf[0] << 8) | buf[1];
}

void ByteStream::operator >> (uint16_t& data)
{
    unsigned char buf[2];

    ReadByte(buf, 2);

    data = (buf[0] << 8) | buf[1];
}

void ByteStream::operator >> (int32_t& data)
{
    unsigned char buf[4];

    ReadByte(buf, 4);

    data = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
}

void ByteStream::operator >> (uint32_t& data)
{
    unsigned char buf[4];

    ReadByte(buf, 4);

    data = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
}

void ByteStream::WriteString(const char *str)
{
    uint32_t size = str ? (uint32_t)strlen(str) : 0;

    *this << size;
    WriteByte((void*)str, size);
}

void ByteStream::WriteString(const char *str, uint32_t len)
{
    *this << len;
    WriteByte((void*)str, len);
}

char* ByteStream::ReadString(uint32_t& len)
{
    *this >> len;
    char* pStr = (char*)GetBuf() + GetPos();
    Skip(len);
    return pStr;
}

void ByteStream::WriteData(uchar_t *data, uint32_t len)
{
    *this << len;
    WriteByte(data, len);
}

uchar_t* ByteStream::ReadData(uint32_t &len)
{
    *this >> len;
    uchar_t* pData = (uchar_t*)GetBuf() + GetPos();
    Skip(len);
    return pData;
}

void ByteStream::ReadByte(void* buf, uint32_t len)
{
    if (pos_ + len > len_)
    {
        throw PduException(ERROR_CODE_PARSE_FAILED, "parase packet failed!");
    }

    if (simpleBuf_)
        simpleBuf_->Read((char*)buf, len);
    else
        memcpy(buf, buf_ + pos_, len);

    pos_ += len;
}

void ByteStream::WriteByte(void* buf, uint32_t len)
{
    if (buf_ && (pos_ + len > len_))
        return;

    if (simpleBuf_)
        simpleBuf_->Write((char*)buf, len);
    else
        memcpy(buf_ + pos_, buf, len);

    pos_ += len;
}
