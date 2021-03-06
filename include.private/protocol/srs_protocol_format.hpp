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

#ifndef SRS_PROTOCOL_FORMAT_HPP
#define SRS_PROTOCOL_FORMAT_HPP

#include <core/srs_core.hpp>

#include <kernel/srs_kernel_codec.hpp>

class SrsOnMetaDataPacket;
class SrsSharedPtrMessage;

/**
 * Create special structure from RTMP stream, for example, the metadata.
 */
class SrsRtmpFormat : public SrsFormat
{
public:
    SrsRtmpFormat();
    virtual ~SrsRtmpFormat();
public:
    // Initialize the format from metadata, optional.
    virtual srs_error_t on_metadata(SrsOnMetaDataPacket* meta);
    // When got a parsed audio packet.
    virtual srs_error_t on_audio(SrsSharedPtrMessage* shared_audio);
    virtual srs_error_t on_audio(int64_t timestamp, char* data, int size);
    // When got a parsed video packet.
    virtual srs_error_t on_video(SrsSharedPtrMessage* shared_video);
    virtual srs_error_t on_video(int64_t timestamp, char* data, int size);
};

#endif

