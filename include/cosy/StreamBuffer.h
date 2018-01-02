// Copyright 2017 Hakan Metin

#ifndef SAT_CORE_STREAMBUFFER_H_
#define SAT_CORE_STREAMBUFFER_H_

#include <zlib.h>

#include <cctype>
#include <string>

static const int kBufferSize = 4096;

namespace cosy {

enum StreamBufferError {
    SUCCESS,
    CANNOT_OPEN_FILE,
    ERROR_PARSE_INT
};

class StreamBuffer {
 public:
    explicit StreamBuffer(const std::string& filename);
    explicit StreamBuffer(const char * filename);
    ~StreamBuffer();

    int readInt();
    void skipWhiteSpaces();
    void skipLine();

    int operator*();
    void operator++();

    bool isValid() const;
    StreamBufferError error() const;
    std::string errorMessage() const;

 private:
    const std::string _filename;
    gzFile _in;
    unsigned char _buffer[kBufferSize];
    unsigned int _index;
    unsigned int _size;
    StreamBufferError _error;

    unsigned char read();
};

}  // namespace cosy


#endif  // SAT_CORE_STREAMBUFFER_H_
