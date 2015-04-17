/*
 * Copyright (c) 2009-2010, Dennis Wang (dennis.wangkk@gmail.com)
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the University of California, Berkeley nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <cstdarg>
#include <cstring>
#include <cstdio>
#include <stdexcept>
#include <cassert>
#include "os_misc.hpp"
#include "log.hpp"
#include "os_misc.hpp"

#ifdef WIN32
#define snprintf _snprintf
#endif

static const char* LevelString[] = {
    0, "[emer]", "[aler]", "[erro]", "[warn]", "[noti]", "[info]", "[debg]"
};
static const int LevelStringLength[] = {
    0,  6,        6,        6,        6,        6,        6,        6
};

Log::Log (const std::string& path_, int level_, bool flush_)
: _fd (path_.c_str (), std::ios::app | std::ios::binary), _path (path_),
  _level (level_), _flush (flush_) {
    if (!_fd) {
        throw std::runtime_error ("cannot open configure file");
    }
}

Log::~Log () {
    (void) _fd.close ();
}

///////////////////////////////////////////////////////////////////////////////////
// mostly, i use C buffer to format string are considering the performance issue //
///////////////////////////////////////////////////////////////////////////////////

void Log::writeLog (int level_, const char* fmt_, ...) {
    assert (fmt_);

    if (_level < level_) {
        return;
    }

    char buf[MAX_LOG_LINE];
    int m = LevelStringLength[level_];
    memcpy (buf, LevelString[level_], m);

    va_list vas;
    va_start (vas, fmt_);
    int n =  vsnprintf (buf + m, MAX_LOG_LINE - m - 1, fmt_, vas);
    if (0 > n) {
        return;
    }
    va_end (vas);

    buf[n + m] = '\n';

    boost::mutex::scoped_lock lock (_mutex);
    _fd.write (buf, n + m + 1);
    if (_flush) {
        _fd.flush ();
    }
}

void Log::writeLog (int level_, int prefix_, const char* fmt_, ...) {
    assert (fmt_);

    if (_level < level_) {
        return;
    }

    char buf[MAX_LOG_LINE];
    int n = 0;
    int m = LevelStringLength[level_];
    memcpy (buf, LevelString[level_], m);

    boost::mutex::scoped_lock lock (_mutex);

    n = _writeLogPrefix (prefix_, buf + m);
    if (0 > n) {
        return;
    }
    m += n;

    va_list vas;
    va_start (vas, fmt_);
    n =  vsnprintf (buf + m, MAX_LOG_LINE - m - 1, fmt_, vas);
    if (0 > n) {
        return;
    }
    va_end (vas);

    buf[n + m] = '\n';

    _fd.write (buf, n + m + 1);
    if (_flush) {
        _fd.flush ();
    }
}

void Log::writeLog (int level_, int prefix_, const char* file_,
    const char* func_, int line_, const char* fmt_, ...) {
    assert (fmt_);

    if (_level < level_) {
        return;
    }

    char buf[MAX_LOG_LINE];
    int n = 0;
    int m = LevelStringLength[level_];
    memcpy (buf, LevelString[level_], m);

    boost::mutex::scoped_lock lock (_mutex);

    n = _writeLogPrefix (prefix_, buf + m);
    if (0 > n) {
        return;
    }
    m += n;

    n = snprintf (buf + m, MAX_LOG_LINE - m, "[%s:%s:%d]", file_, func_, line_);
    if (0 > n) {
        return;
    }
    m += n;

    va_list vas;
    va_start (vas, fmt_);
    n =  vsnprintf (buf + m, MAX_LOG_LINE - m - 1, fmt_, vas);
    if (0 > n) {
        return;
    }
    va_end (vas);

    buf[n + m] = '\n';

    _fd.write (buf, n + m + 1);
    if (_flush) {
        _fd.flush ();
    }
}

void Log::writeLog (int level_, const char* fmt_, va_list vas_) {
    assert (fmt_);

    if (_level < level_) {
        return;
    }

    char buf[MAX_LOG_LINE];
    int m = LevelStringLength[level_];
    memcpy (buf, LevelString[level_], m);

    int n =  vsnprintf (buf + m, MAX_LOG_LINE - m - 1, fmt_, vas_);
    if (0 > n) {
        return;
    }

    buf[n + m] = '\n';

    boost::mutex::scoped_lock lock (_mutex);
    _fd.write (buf, n + m + 1);
    if (_flush) {
        _fd.flush ();
    }
}

void Log::writeLog (int level_, int prefix_, const char* fmt_, va_list vas_) {
    assert (fmt_);

    if (_level < level_) {
        return;
    }

    char buf[MAX_LOG_LINE];
    int n = 0;
    int m = LevelStringLength[level_];
    memcpy (buf, LevelString[level_], m);

    boost::mutex::scoped_lock lock (_mutex);

    n = _writeLogPrefix (prefix_, buf + m);
    if (0 > n) {
        return;
    }
    m += n;

    n =  vsnprintf (buf + m, MAX_LOG_LINE - m - 1, fmt_, vas_);
    if (0 > n) {
        return;
    }

    buf[n + m] = '\n';

    _fd.write (buf, n + m + 1);
    if (_flush) {
        _fd.flush ();
    }
}

void Log::writeLog (int level_, int prefix_, const char* file_, const char* func_,
    int line_, const char* fmt_, va_list vas_) {
    assert (fmt_);

    if (_level < level_) {
        return;
    }

    char buf[MAX_LOG_LINE];
    int n = 0;
    int m = LevelStringLength[level_];
    memcpy (buf, LevelString[level_], m);

    boost::mutex::scoped_lock lock (_mutex);

    n = _writeLogPrefix (prefix_, buf + m);
    if (0 > n) {
        return;
    }
    m += n;

    n = snprintf (buf + m, MAX_LOG_LINE - m, "[%s:%s:%d]", file_, func_, line_);
    if (0 > n) {
        return;
    }
    m += n;

    n =  vsnprintf (buf + m, MAX_LOG_LINE - m - 1, fmt_, vas_);
    if (0 > n) {
        return;
    }

    buf[n + m] = '\n';

    _fd.write (buf, n + m + 1);
    if (_flush) {
        _fd.flush ();
    }
}

int Log::_writeLogPrefix (int prefix_, char* buffer_) {
    int m = 0;
    int n = 0;

    if (PID & prefix_) {
        n = snprintf (buffer_ + m, MAX_LOG_LINE - m, "[%d]", OS::getpid ());
        if (0 > n) {
            return -1;
        }
        m += n;
    }

    if (TID & prefix_) {
        n = snprintf (buffer_ + m, MAX_LOG_LINE - m, "[%d]", OS::gettid ());
        if (0 > n) {
            return -1;
        }
        m += n;
    }

    if (TIME & prefix_) {
        struct timeval tv;
        struct tm* time = 0;

        OS::gettimeofday(&tv, 0);
        time = OS::localtime(&tv);

        buffer_[m] = '[';
        m += 1;

        if (YEAR & prefix_) {
            n = snprintf (buffer_ + m, MAX_LOG_LINE - m, "%04d",
                    time->tm_year + 1900);
            if (0 > n) {
                return -1;
            }
            m += n;
        }

        if (MONTH & prefix_) {
            n = snprintf (buffer_ + m, MAX_LOG_LINE - m, "%02d",
                    time->tm_mon + 1);
            if (0 > n) {
                return -1;
            }
            m += n;
        }

        if (DAY & prefix_) {
            n = snprintf (buffer_ + m, MAX_LOG_LINE - m, "%02d", time->tm_mday);
            if (0 > n) {
                return -1;
            }
            m += n;
        }

        if (YMD & prefix_) {
            buffer_[m] = ' ';
            m += 1;

            if (HOUR & prefix_) {
                n = snprintf (buffer_ + m, MAX_LOG_LINE - m, "%02d",
                        time->tm_hour);
                if (0 > n) {
                    return -1;
                }
                m += n;
            }

            if (MINUTE & prefix_) {
                if (HOUR & prefix_) {
                    buffer_[m] = ':';
                    m += 1;
                }
                n = snprintf (buffer_ + m, MAX_LOG_LINE - m, "%02d",
                        time->tm_min);
                if (0 > n) {
                    return -1;
                }
                m += n;
            }

            if (SEC & prefix_) {
                if (HM & prefix_) {
                    buffer_[m] = ':';
                    m += 1;
                }
                n = snprintf (buffer_ + m, MAX_LOG_LINE - m, "%02d",
                        time->tm_sec);
                if (0 > n) {
                    return -1;
                }
                m += n;
            }

            if (USEC & prefix_) {
                if (HMS & prefix_) {
                    buffer_[m] = ':';
                    m += 1;
                }
                n = snprintf (buffer_ + m, MAX_LOG_LINE - m, "%03d",
                        tv.tv_usec / 1000);
                if (0 > n) {
                    return -1;
                }
                m += n;
            }

            if (NSEC & prefix_) {
                if (HMSUS & prefix_) {
                    buffer_[m] = ':';
                    m += 1;
                }
                n = snprintf (buffer_ + m, MAX_LOG_LINE - m, "%03d",
                        tv.tv_usec % 1000);
                if (0 > n) {
                    return -1;
                }
                m += n;
            }
        }

        buffer_[m] = ']';
        m += 1;
    }

    return m;
}
