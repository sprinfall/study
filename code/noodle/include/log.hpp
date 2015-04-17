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

/*!
 \file log.hpp
 \brief logger
 */

#ifndef log_hpp
#define log_hpp

#include <string>
#include <fstream>
#include <boost/thread/mutex.hpp>

/*!
 \brief logger
 */

class Log {
public:

    /*!
     \brief log level
     */

    enum LogLevel {
        EMERG = 1, ///< emergency
        ALERT,     ///< alert
        ERR,       ///< error
        WARN,      ///< warning
        NOTI,      ///< notice
        INFO,      ///< information
        DEBUG      ///< debug
    };

    /*!
     \brief log PREFIX
     */

    enum LogPrefix {
        PID     = 0x00000001,
        TID     = 0x00000002,
        YEAR    = 0x00000004,
        MONTH   = 0x00000008,
        DAY     = 0x00000010,
        YMD     = YEAR | MONTH | DAY,
        HOUR    = 0x00000020,
        MINUTE  = 0x00000040,
        HM      = HOUR | MINUTE,
        SEC     = 0x00000080,
        HMS     = HM | SEC,
        USEC    = 0x00000100,
        HMSUS   = HMS | USEC,
        NSEC    = 0x00000200,
        HMSUSNS = HMSUS | NSEC,
        TIME    = YEAR | MONTH | DAY | HOUR | MINUTE | SEC | USEC | NSEC
    };

public:

    /*!
     \brief constructor
     \param path_ log path
     \param level_ log level
     \param flush_ whether flush I/O buffer for each writing
     */

    Log (const std::string& path_, int level_ = NOTI, bool flush_ = false);

    /*!
     \brief destructor
     */

    ~Log ();

    /*!
     \brief write log
     \param level_ log level
     \param fmt_ fomatted string like fprintf
     */

    void writeLog (int level_, const char* fmt_, ...);

    /*!
     \brief write log
     \param level_ log level
     \param prefix_ the prefix of log line
     \param fmt_ fomatted string like fprintf
     */

    void writeLog (int level_, int prefix_, const char* fmt_, ...);

    /*!
     \brief write log
     \param level_ log level
     \param prefix_ the prefix of log line
     \param file_ file name
     \param func_ function name
     \param line_ line number
     \param fmt_ fomatted string like fprintf
     */

    void writeLog (int level_, int prefix_, const char* file_,
        const char* func_, int line_, const char* fmt_, ...);

    /*!
     \brief write log
     \param level_ log level
     \param fmt_ fomatted string like fprintf
     \param vas_ C va_list
     */

    void writeLog (int level_, const char* fmt_, va_list vas_);

    /*!
     \brief write log
     \param level_ log level
     \param prefix_ the prefix of log line
     \param fmt_ fomatted string like fprintf
     \param vas_ C va_list
     */

    void writeLog (int level_, int prefix_, const char* fmt_, va_list vas_);

    /*!
     \brief write log
     \param level_ log level
     \param prefix_ the prefix of log line
     \param file_ file name
     \param func_ function name
     \param line_ line number
     \param fmt_ fomatted string like fprintf
     \param vas_ C va_list
     */

    void writeLog (int level_, int prefix_, const char* file_,
        const char* func_, int line_, const char* fmt_, va_list vas_);

private:

    /*!
     \brief write PREFIX of log line to buffer
     \param prefix_ PREFIX flag combained with bitwise OR
     \param buffer_ log line buffer
     \return PREFIX length in bytes
     */

    int _writeLogPrefix (int prefix_, char* buffer_);

private:
    std::ofstream _fd;    ///< file descriptor
    bool          _flush; ///< need flush output buffer for each log writing
    int           _level; ///< log level
    std::string   _path;  ///< log path
    boost::mutex  _mutex; ///< binary lock for writing

    static const int MAX_LOG_LINE = 2048;  ///< max log line length
};

#endif // log_hpp
