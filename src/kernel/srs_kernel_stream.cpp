/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2020 Winlin
 * Copyright (c) 2021 BenjaminPMLovegood
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <kernel/srs_kernel_stream.hpp>

#include <kernel/srs_kernel_error.hpp>
#include <kernel/srs_kernel_log.hpp>
#include <kernel/srs_kernel_utility.hpp>
#include <core/srs_core_performance.hpp>

SrsSimpleStream::SrsSimpleStream()
{
}

SrsSimpleStream::~SrsSimpleStream()
{
}

int SrsSimpleStream::length()
{
    int len = (int)data.size();
    srs_assert(len >= 0);
    return len;
}

char* SrsSimpleStream::bytes()
{
    return (length() == 0)? NULL : &data.at(0);
}

void SrsSimpleStream::erase(int size)
{
    if (size <= 0) {
        return;
    }
    
    if (size >= length()) {
        data.clear();
        return;
    }
    
    data.erase(data.begin(), data.begin() + size);
}

void SrsSimpleStream::append(const char* bytes, int size)
{
    if (size > 0) {
        data.insert(data.end(), bytes, bytes + size);
    }
}

void SrsSimpleStream::append(SrsSimpleStream* src)
{
    append(src->bytes(), src->length());
}
