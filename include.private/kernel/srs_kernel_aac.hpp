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

#ifndef SRS_KERNEL_AAC_HPP
#define SRS_KERNEL_AAC_HPP

#include <core/srs_core.hpp>

#if !defined(SRS_EXPORT_LIBRTMP)

#include <string>

#include <kernel/srs_kernel_codec.hpp>

class SrsBuffer;
class ISrsStreamWriter;

// Transmux the RTMP packets to AAC stream.
class SrsAacTransmuxer
{
private:
    ISrsStreamWriter* writer;
private:
    SrsAacObjectType aac_object;
    int8_t aac_sample_rate;
    int8_t aac_channels;
    bool got_sequence_header;
public:
    SrsAacTransmuxer();
    virtual ~SrsAacTransmuxer();
public:
    // Initialize the underlayer file stream.
    // @remark User can initialize multiple times to encode multiple aac files.
    // @remark User must free the fs, aac encoder never close/free it.
    virtual srs_error_t initialize(ISrsStreamWriter* fs);
public:
   // Write audio/video packet.
   // @remark The assert data should not be NULL.
    virtual srs_error_t write_audio(int64_t timestamp, char* data, int size);
};

#endif

#endif

