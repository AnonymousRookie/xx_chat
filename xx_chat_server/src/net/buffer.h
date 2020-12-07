#ifndef Z_NET_SRC_BUFFER_H
#define Z_NET_SRC_BUFFER_H

#include <algorithm>
#include <vector>
#include <string.h>
#include <string>
#include "z_endian.h"

NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(net)

// A buffer class modeled after org.jboss.netty.buffer.ChannelBuffer
//
// +-------------------+------------------+------------------+
// | prependable bytes |  readable bytes  |  writable bytes  |
// |                   |     (CONTENT)    |                  |
// +-------------------+------------------+------------------+
// |                   |                  |                  |
// 0      <=      readerIndex   <=   writerIndex    <=     size

class Buffer
{
public:
    static const size_t kCheapPrepend = 8;
    static const size_t kInitialSize = 1024;

    explicit Buffer(size_t initialSize = kInitialSize);

    void swap(Buffer& rhs);
    size_t readableBytes() const;
    size_t writableBytes() const;
    size_t prependableBytes() const;
    const char* peek() const;
    const char* findCRLF() const;
    const char* findCRLF(const char* start) const;
    const char* findEOL() const;
    const char* findEOL(const char* start) const;
    void retrieve(size_t len);
    void retrieveUntil(const char* end);
    void retrieveInt64();
    void retrieveInt32();
    void retrieveInt16();
    void retrieveInt8();
    void retrieveAll();
    std::string retrieveAllAsString();
    std::string retrieveAsString(size_t len);
    std::string toStringPiece() const;
    void append(const std::string& str);
    void append(const char* /*restrict*/ data, size_t len);
    void append(const void* /*restrict*/ data, size_t len);
    void ensureWritableBytes(size_t len);
    char* beginWrite();
    const char* beginWrite() const;
    void hasWritten(size_t len);
    void unwrite(size_t len);
    // Append int64_t using network endian
    void appendInt64(int64_t x);
    // Append int32_t using network endian
    void appendInt32(int32_t x);
    void appendInt16(int16_t x);
    void appendInt8(int8_t x);
    // Read int64_t from network endian
    int64_t readInt64();
    // Read int32_t from network endian
    int32_t readInt32();
    int16_t readInt16();
    int8_t readInt8();
    // Peek int64_t from network endian
    int64_t peekInt64() const;
    // Peek int32_t from network endian
    int32_t peekInt32() const;
    int16_t peekInt16() const;
    int8_t peekInt8() const;
    // Prepend int64_t using network endian
    void prependInt64(int64_t x);
    // Prepend int32_t using network endian
    void prependInt32(int32_t x);
    void prependInt16(int16_t x);
    void prependInt8(int8_t x);
    void prepend(const void* /*restrict*/ data, size_t len);
    void shrink(size_t reserve);
    size_t internalCapacity() const;
    // Read data directly into buffer.
    ssize_t readFd(int fd, int* savedErrno);

private:
    char* begin();
    const char* begin() const;
    void makeSpace(size_t len);

private:
    std::vector<char> buffer_;
    size_t readerIndex_;
    size_t writerIndex_;
    static const char kCRLF[];
};

NAMESPACE_END(net)
NAMESPACE_END(z)

#endif  // Z_NET_SRC_BUFFER_H