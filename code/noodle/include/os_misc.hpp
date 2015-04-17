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
 \file os_misc.hpp
 \brief Operating system relative C function
 */

#ifndef os_misc_hpp
#define os_misc_hpp

#include <string>

#ifdef WIN32
#include <ctime>
#include <windows.h>
#define Handle HMODULE
#else
#include <cstdlib>
#include <ctime>
#include <sys/time.h>
#include <pthread.h>
#include <dlfcn.h>
#include <signal.h>
#include <sys/syscall.h>
#include <unistd.h>
#define Handle void*
#endif

/*!
 \brief Operating system relative C function
 */

class OS {
public:

    /*!
     \brief get environment variable value by name
     \param name_ environment variable name
     \return environment variable value
     */

    static char* getEnv (const char* name_);

    /*!
     \brief to be a daemon
     \return 0 or error code
     */

    static int daemon ();

    /*!
     \brief get process ID
     \return process ID
     */

    static int getpid ();

    /*!
     \brief get thread ID
     \return thread ID
     */

    static int gettid ();

    /*!
     \brief open dynamic linked library
     \param path_ library path
     \return the handle of library
     */

    static Handle dlopen (const char* path_);

    /*!
     \brief close dynamic linked library
     \param handle_ the handle of library
     */

    static void dlclose (Handle handle_);

    /*!
     \brief get export function address of library
     \param handle_ the handle of library
     \param name_ function name
     \return function address
     */

    static void* dlsym (Handle handle_, const char* name_);

    /*!
     \brief fill struct timeval
     \param tp_ struct timeval pointer
     \param tzp_ reserved
     \return 0 or error code
     */

    static int gettimeofday (struct timeval *tp_, void *tzp_);

    /*!
     \brief fill struct tm
     \param tv_ struct timeval pointer
     \return filled struct tm pointer
     */

    static struct tm* localtime (struct timeval* tv_);

    /*!
     \brief fill struct tm
     \return filled struct tm pointer
     */

    static struct tm* getLocalTime ();

    /*!
     \brief add '\\' or '/' at the end of path
     \param path_ path
     \return modified path
     */

    static std::string validatePath (const std::string& path_);

    /*!
     \brief make thread sleep
     \param utime_ millionseconds
     */

    static void sleep (int utime_);

    /*!
     \brief set process signal handler
     */

    static void setCtrlHandler ();

    /*!
     \brief set the seed of  random generator
     \param seed_ seed
     */

    static void setRandomSeed (unsigned seed_);

    /*!
     \brief get random number
     \return random number of range [0, max-signed-short)
     */
    
    static unsigned getRandom ();

    /*!
     \brief lauch a new process
     \param path_ binary path
     \param argv_ command line argument vector
     \return OK or error code
     */

    static int createProcess (const char* path_, char* const* argv_);
};

#endif // os_misc_hpp
