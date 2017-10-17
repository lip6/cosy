/* Copyright 2017 Hakan Metin - All rights reserved */

#ifndef INCLUDE_DSB_STREAMBUFFER_H_
#define INCLUDE_DSB_STREAMBUFFER_H_

#include <zlib.h>
#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <string>

static const int BUFFER_SIZE = 4096;

class StreamBuffer {
 public:
        StreamBuffer();
        explicit StreamBuffer(const std::string filename);
        explicit StreamBuffer(const char * filename);
        ~StreamBuffer();

        bool valid() const;
        bool eof() const;
        int operator*();
        void operator++();
        int parseInt();
        void skipLine();
        void skipWhiteSpace();

 private:
        gzFile _in;
        unsigned char _buffer[BUFFER_SIZE];
        unsigned int _index;
        unsigned int _size;

        void nextChar();
        void readLineIfNeed();
};
#endif  // INCLUDE_DSB_STREAMBUFFER_H_

/*
 * Local Variables:
 * mode: c++
 * indent-tabs-mode: nil
 * End:
 */
