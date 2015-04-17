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

#include <cassert>
#include <cstdlib>

#include "core.hpp"
#include "global_object_manager.hpp"
#include "os_misc.hpp"

char* OS::getEnv (const char* name_) {
    assert (name_);
    return ::getenv (name_);
}

int OS::daemon () {
#ifdef WIN32
    return 0;
#else

    // keep current work DIR

    return ::daemon (1, 0);
#endif
}

int OS::getpid () {
#ifdef WIN32
    return (int)::GetCurrentProcessId ();
#else
    return ::getpid ();
#endif
}

int OS::gettid () {
#ifdef WIN32
    return (int)::GetCurrentThreadId ();
#else
    return ::syscall(__NR_gettid);
#endif
}

Handle OS::dlopen (const char* path_) {
#ifdef WIN32
    return ::LoadLibraryA (path_);
#else
    return ::dlopen (path_, RTLD_LAZY);
#endif
}

void OS::dlclose (Handle handle_) {
#ifdef WIN32
    (BOOL) ::FreeLibrary (handle_);
#else
    ::dlclose (handle_); 
#endif
}

void* OS::dlsym   (Handle handle_, const char* name_) {
#ifdef WIN32
    return ::GetProcAddress (handle_, name_);
#else
    return ::dlsym (handle_, name_);
#endif
}

int OS::gettimeofday (struct timeval *tp, void *tzp) {
    (void) tzp;

#ifdef WIN32
    time_t clock;
    struct tm tm;
    SYSTEMTIME st;
    ::GetLocalTime(&st);

    tm.tm_year  = st.wYear - 1900;
    tm.tm_mon   = st.wMonth - 1;
    tm.tm_mday  = st.wDay;
    tm.tm_hour  = st.wHour;
    tm.tm_min   = st.wMinute;
    tm.tm_sec   = st.wSecond;
    tm.tm_isdst = -1;
    clock       = ::mktime(&tm);
    tp->tv_sec  = (long)clock;
    tp->tv_usec = st.wMilliseconds * 1000;

    return 0;
#else
    return ::gettimeofday (tp, 0);
#endif
}

struct tm* OS::localtime (struct timeval* tv_) {
#ifdef WIN32
    time_t second = (time_t)(tv_->tv_sec);
    return ::localtime (&second);
#else
    return ::localtime (&tv_->tv_sec);
#endif
}

struct tm* OS::getLocalTime () {
    struct timeval tv;
    OS::gettimeofday (&tv, 0);
    return OS::localtime (&tv);
}

std::string OS::validatePath (const std::string& path_) {
#ifdef WIN32
    char pathSplit = '\\';
#else
    char pathSplit = '/';
#endif
    if (pathSplit == path_.at(path_.length () - 1)) {
        return path_;
    }

    std::string modifiedPath = path_;

    return modifiedPath += pathSplit;
}

void OS::sleep (int utime_) {
#ifdef WIN32
    ::Sleep (utime_);
#else
    ::usleep (utime_);
#endif
}

#ifdef WIN32
BOOL
WINAPI
HandlerRoutine(__in DWORD dwCtrlType) {
    switch (dwCtrlType) {
        case CTRL_C_EVENT:
        case CTRL_BREAK_EVENT:
        case CTRL_CLOSE_EVENT:
        case CTRL_SHUTDOWN_EVENT: {
            GlobalObjectManager::getInstance ()->getObject<Core*>("core")->halt ();
            return TRUE;
        }
        default:
            break;
    }
    return FALSE;
}
#else

// signal handler for SIGTERM, SIGQUIT

void SIGTERM_Handler (int sig_) {
    (void) sig_;
    GlobalObjectManager::getInstance ()->getObject<Core*>(
        "core")->postPending ();
}

void SIGQUIT_Handler (int sig_) {
    (void) sig_;
    GlobalObjectManager::getInstance ()->getObject<Core*>(
        "core")->postPending ();
}

void SIGUSR1_Handler (int sig_) {
    (void) sig_;
    GlobalObjectManager::getInstance ()->getObject<Core*>(
        "core")->postRestart ();
}

void SIGUSR2_Handler (int sig_) {
    (void) sig_;
    GlobalObjectManager::getInstance ()->getObject<Core*>(
        "core")->postReloadConfig ();
}

#endif // WIN32

void OS::setCtrlHandler () {
#ifdef WIN32
    ::SetConsoleCtrlHandler (HandlerRoutine, TRUE);
#else

    struct sigaction SIGTERMAction;
    memset (&SIGTERMAction, 0, sizeof (SIGTERMAction));
    SIGTERMAction.sa_handler = SIGTERM_Handler;
    SIGTERMAction.sa_flags = SA_RESTART;

    sigaction (SIGTERM, &SIGTERMAction, 0);

    struct sigaction SIGQUITAction;
    memset (&SIGQUITAction, 0, sizeof (SIGQUITAction));
    SIGQUITAction.sa_handler = SIGQUIT_Handler;
    SIGQUITAction.sa_flags = SA_RESTART;

    sigaction (SIGQUIT, &SIGQUITAction, 0);

    struct sigaction SIGUSR1Action;
    memset (&SIGUSR1Action, 0, sizeof (SIGUSR1Action));
    SIGUSR1Action.sa_handler = SIGUSR1_Handler;
    SIGUSR1Action.sa_flags = SA_RESTART;

    sigaction (SIGUSR1, &SIGUSR1Action, 0);

    struct sigaction SIGUSR2Action;
    memset (&SIGUSR2Action, 0, sizeof (SIGUSR2Action));
    SIGUSR2Action.sa_handler = SIGUSR2_Handler;
    SIGUSR2Action.sa_flags = SA_RESTART;

    sigaction (SIGUSR2, &SIGUSR2Action, 0);

#endif // WIN32
}

void OS::setRandomSeed (unsigned seed_) {
#ifdef WIN32
    ::srand (seed_);
#else
    ::srandom (seed_);
#endif
}

unsigned OS::getRandom () {
#ifdef WIN32
    return (unsigned)::rand ();
#else
    return (unsigned)::random ();
#endif
}

int OS::createProcess (const char* path_, char* const* argv_) {
#ifdef WIN32
    return FAIL;
#else
    return (execv (path_, argv_) > 0 ? OK : FAIL);
#endif // WIN32
}
