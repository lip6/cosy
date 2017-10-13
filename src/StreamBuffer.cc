/* Copyright 2017 Hakan Metin - All rights reserved */

#include "cosy/StreamBuffer.h"

StreamBuffer::StreamBuffer() :
    _in(nullptr),
    _index(0),
    _size(0) {
}

StreamBuffer::StreamBuffer(const std::string filename) :
    StreamBuffer(filename.c_str()) {
}

StreamBuffer::StreamBuffer(const char* filename) :
    _in(nullptr),
    _index(0),
    _size(0) {
    _in = gzopen(filename, "rb");
    readLineIfNeed();
}

StreamBuffer::~StreamBuffer() {
    if (_in != nullptr) {
        gzclose(_in);
    }
}

bool StreamBuffer::ok() const {
    return _in != nullptr;
}

bool StreamBuffer::eof() const {
    return _buffer[_index] == '\0' || _size == 0;
}

int StreamBuffer::operator*() {
    return (_index >= _size) ? EOF : _buffer[_index];
}

void StreamBuffer::operator++() {
    nextChar();
}

int StreamBuffer::parseInt() {
    bool negative = false;
    int value = 0;

    skipWhiteSpace();

    if (_buffer[_index] == '-') {
        negative = true;
        nextChar();
    } else if (_buffer[_index] == '+') {
        nextChar();
    }
    while (!eof() && _buffer[_index] >= '0' && _buffer[_index] <= '9') {
        value = (value * 10) + (_buffer[_index] - '0');
        nextChar();
    }

    return negative ? -value : value;
}

void StreamBuffer::skipLine() {
    while (!eof() && _buffer[_index] != '\n')
        nextChar();
    if (!eof() && _buffer[_index] == '\n')
        nextChar();
}

void StreamBuffer::nextChar() {
    ++_index;
    readLineIfNeed();
}

void StreamBuffer::readLineIfNeed() {
    if (_index >= _size) {
        _size = gzread(_in, _buffer, BUFFER_SIZE);
        _index = 0;
    }
}

void StreamBuffer::skipWhiteSpace() {
    unsigned char c = _buffer[_index];

    while (!eof() && ((c >= 9 && c <= 13) || c == 32)) {
        nextChar();
        c = _buffer[_index];
    }
}
