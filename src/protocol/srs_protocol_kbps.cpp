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

#include <protocol/srs_protocol_kbps.hpp>

#include <kernel/srs_kernel_utility.hpp>

SrsKbpsSample::SrsKbpsSample()
{
    bytes = time = -1;
    kbps = 0;
}

SrsKbpsSample::~SrsKbpsSample()
{
}

SrsKbpsSample* SrsKbpsSample::update(int64_t b, srs_utime_t t, int k)
{
    bytes = b;
    time = t;
    kbps = k;
    return this;
}

SrsKbpsSlice::SrsKbpsSlice(SrsWallClock* c)
{
    clk = c;
    io = NULL;
    last_bytes = io_bytes_base = starttime = bytes = delta_bytes = 0;
}

SrsKbpsSlice::~SrsKbpsSlice()
{
}

int64_t SrsKbpsSlice::get_total_bytes()
{
    return bytes + last_bytes - io_bytes_base;
}

void SrsKbpsSlice::sample()
{
    srs_utime_t now = clk->now();
    int64_t total_bytes = get_total_bytes();
    
    if (sample_30s.time < 0) {
        sample_30s.update(total_bytes, now, 0);
    }
    if (sample_1m.time < 0) {
        sample_1m.update(total_bytes, now, 0);
    }
    if (sample_5m.time < 0) {
        sample_5m.update(total_bytes, now, 0);
    }
    if (sample_60m.time < 0) {
        sample_60m.update(total_bytes, now, 0);
    }
    
    if (now - sample_30s.time >= 30 * SRS_UTIME_SECONDS) {
        int kbps = (int)((total_bytes - sample_30s.bytes) * 8 / srsu2ms(now - sample_30s.time));
        sample_30s.update(total_bytes, now, kbps);
    }
    if (now - sample_1m.time >= 60 * SRS_UTIME_SECONDS) {
        int kbps = (int)((total_bytes - sample_1m.bytes) * 8 / srsu2ms(now - sample_1m.time));
        sample_1m.update(total_bytes, now, kbps);
    }
    if (now - sample_5m.time >= 300 * SRS_UTIME_SECONDS) {
        int kbps = (int)((total_bytes - sample_5m.bytes) * 8 / srsu2ms(now - sample_5m.time));
        sample_5m.update(total_bytes, now, kbps);
    }
    if (now - sample_60m.time >= 3600 * SRS_UTIME_SECONDS) {
        int kbps = (int)((total_bytes - sample_60m.bytes) * 8 / srsu2ms(now - sample_60m.time));
        sample_60m.update(total_bytes, now, kbps);
    }
}

ISrsKbpsDelta::ISrsKbpsDelta()
{
}

ISrsKbpsDelta::~ISrsKbpsDelta()
{
}

SrsWallClock::SrsWallClock()
{
}

SrsWallClock::~SrsWallClock()
{
}

srs_utime_t SrsWallClock::now()
{
    return srs_get_system_time();
}

SrsKbps::SrsKbps(SrsWallClock* c) : is(c), os(c)
{
    clk = c;
}

SrsKbps::~SrsKbps()
{
}

void SrsKbps::set_io(ISrsProtocolStatistic* in, ISrsProtocolStatistic* out)
{
    // set input stream
    // now, set start time.
    if (is.starttime == 0) {
        is.starttime = clk->now();
    }
    // save the old in bytes.
    if (is.io) {
        is.bytes += is.io->get_recv_bytes() - is.io_bytes_base;
    }
    // use new io.
    is.io = in;
    is.last_bytes = is.io_bytes_base = 0;
    if (in) {
        is.last_bytes = is.io_bytes_base = in->get_recv_bytes();
    }
    // resample
    is.sample();
    
    // set output stream
    // now, set start time.
    if (os.starttime == 0) {
        os.starttime = clk->now();
    }
    // save the old in bytes.
    if (os.io) {
        os.bytes += os.io->get_send_bytes() - os.io_bytes_base;
    }
    // use new io.
    os.io = out;
    os.last_bytes = os.io_bytes_base = 0;
    if (out) {
        os.last_bytes = os.io_bytes_base = out->get_send_bytes();
    }
    // resample
    os.sample();
}

int SrsKbps::get_send_kbps()
{
    int duration = srsu2ms(clk->now() - is.starttime);
    if (duration <= 0) {
        return 0;
    }

    int64_t bytes = get_send_bytes();
    return (int)(bytes * 8 / duration);
}

int SrsKbps::get_recv_kbps()
{
    int duration = srsu2ms(clk->now() - os.starttime);
    if (duration <= 0) {
        return 0;
    }

    int64_t bytes = get_recv_bytes();
    return (int)(bytes * 8 / duration);
}

int SrsKbps::get_send_kbps_30s()
{
    return os.sample_30s.kbps;
}

int SrsKbps::get_recv_kbps_30s()
{
    return is.sample_30s.kbps;
}

int SrsKbps::get_send_kbps_5m()
{
    return os.sample_5m.kbps;
}

int SrsKbps::get_recv_kbps_5m()
{
    return is.sample_5m.kbps;
}

void SrsKbps::add_delta(int64_t in, int64_t out)
{
    // update the total bytes
    is.last_bytes += in;
    os.last_bytes += out;
    
    // we donot sample, please use sample() to do resample.
}

void SrsKbps::sample()
{
    // update the total bytes
    if (os.io) {
        os.last_bytes = os.io->get_send_bytes();
    }
    
    if (is.io) {
        is.last_bytes = is.io->get_recv_bytes();
    }
    
    // resample
    is.sample();
    os.sample();
}

int64_t SrsKbps::get_send_bytes()
{
    // we must calc the send bytes dynamically,
    // to not depends on the sample(which used to calc the kbps).
    // @read https://github.com/ossrs/srs/issues/588
    
    // session start bytes.
    int64_t bytes = os.bytes;
    
    // When exists active session, use it to get the last bytes.
    if (os.io) {
        bytes += os.io->get_send_bytes() - os.io_bytes_base;
        return bytes;
    }
    
    // When no active session, the last_bytes record the last valid bytes.
    // TODO: Maybe the bellow bytes is zero, because the ios.io.out is NULL.
    bytes += os.last_bytes - os.io_bytes_base;
    
    return bytes;
}

int64_t SrsKbps::get_recv_bytes()
{
    // we must calc the send bytes dynamically,
    // to not depends on the sample(which used to calc the kbps).
    // @read https://github.com/ossrs/srs/issues/588
    
    // session start bytes.
    int64_t bytes = is.bytes;
    
    // When exists active session, use it to get the last bytes.
    if (is.io) {
        bytes += is.io->get_recv_bytes() - is.io_bytes_base;
        return bytes;
    }
    
    // When no active session, the last_bytes record the last valid bytes.
    // TODO: Maybe the bellow bytes is zero, because the ios.io.out is NULL.
    bytes += is.last_bytes - is.io_bytes_base;
    
    return bytes;
}

void SrsKbps::remark(int64_t* in, int64_t* out)
{
    sample();
    
    int64_t inv = is.get_total_bytes() - is.delta_bytes;
    is.delta_bytes = is.get_total_bytes();
    if (in) {
        *in = inv;
    }
    
    int64_t outv = os.get_total_bytes() - os.delta_bytes;
    os.delta_bytes = os.get_total_bytes();
    if (out) {
        *out = outv;
    }
}

int SrsKbps::size_memory()
{
    return sizeof(SrsKbps);
}

