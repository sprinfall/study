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

#ifndef noodle_hpp
#define noodle_hpp

enum ErrorCode {
    OK = 0,
    FAIL,
    DONE,
    ERR_BAD_ALLOC,
    ERR_UNDEFINED_STATE,
    ERR_NEED_ATTACH,
    ERR_SAME_SUBJECT,
    ERR_UNDEFINED_INSTANCE,
    ERR_UNKNOWN_MODULE,
    ERR_LOAD_MODULE,
    ERR_UNDEFINED_MODULE_PROC,
    ERR_DEAD_SUBJECT,
    ERR_DUP_MODULE,
    ERR_DUP_INSTANCE,
    ERR_UNKNOWN_INSTANCE,
    ERR_UNKNOWN_PROXY,
    ERR_DUP_PROXY,
    ERR_DISABLED_SUBJECT,
    ERR_UNIMPL
};

enum State {
    STATE_ACTIVE = 1,
    STATE_INACTIVE,
    STATE_SUSPEND,
    STATE_DISABLE
};

enum MessageType {
    MSG_INSTALL = 1,
    MSG_INSTALL_SOURCE,
    MSG_UNINSTALL,
    MSG_DISABLE,
    MSG_ENABLE,
    MSG_CREATE,
    MSG_DESTROY,
    MSG_UPDATE,
    MSG_SHUTDOWN,
    MSG_REBOOT,
    MSG_STAT,
    MSG_CMD_MAX,
    MSG_QUIT,
    MSG_CALL,
    MSG_TRACE_CALL
};

enum LogLevel {
    EMERG = 1,
    ALERT,
    ERR,
    WARN,
    NOTI,
    INFO,
    DEBUG
};

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

#ifdef WIN32

#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#define _CRT_SECURE_NO_DEPRECATE
#endif

#   define writeDebugLog(fmt, ...) \
        writeLog (DEBUG, PID | TID | TIME, __FILE__, \
            __FUNCTION__, __LINE__, fmt, __VA_ARGS__)

#   define writeErrorLog(fmt, ...) \
        writeLog (ERR, PID | TID | TIME, fmt, __VA_ARGS__)

#   define writeNoticeLog(fmt, ...) \
        writeLog (NOTI, PID | TID | TIME, fmt, __VA_ARGS__)

#   define writeWarnLog(fmt, ...) \
        writeLog (WARN, PID | TID | TIME, fmt, __VA_ARGS__)

#else

#   define writeDebugLog(fmt...) \
        writeLog (DEBUG, PID | TID | TIME, __FILE__, \
            __FUNCTION__, __LINE__, ##fmt)

#   define writeErrorLog(fmt...) \
        writeLog (ERR, PID | TID | TIME, ##fmt)

#   define writeNoticeLog(fmt...) \
        writeLog (NOTI, PID | TID | TIME, ##fmt)

#   define writeWarnLog(fmt...) \
        writeLog (WARN, PID | TID | TIME, ##fmt)

#endif // WIN32

static const int MAX_MODULE_NAME = 64;
static const int MAX_MODULE_PATH = 512;
static const int MAX_STAT_BUFFER = 102400;

struct CommandBasic {
    int instanceID;
};

struct CommandStart {
    int  instanceID;
    char pluginName[MAX_MODULE_NAME];
};

struct CommandInstall {
    bool source;
    char pluginName[MAX_MODULE_NAME];
    char pluginPath[MAX_MODULE_PATH];
};

struct CommandUninstall {
    char pluginName[MAX_MODULE_NAME];
};

struct CommandUpdate {
    char oldPluginName[MAX_MODULE_NAME];
    char newPluginName[MAX_MODULE_NAME];
    char pluginPath[MAX_MODULE_PATH];
};

struct CommandStat {
    bool printTree;
    bool printConfig;
    bool printMessage;
    bool printVersion;
    bool printPool;
    char statBuffer[MAX_STAT_BUFFER];
};

struct Command {
    int  msgID;
    union {
        CommandBasic     cmdBasic;
        CommandStart     cmdStart;
        CommandInstall   cmdInstall;
        CommandUninstall cmdUninstall;
        CommandUpdate    cmdUpdate;
        CommandStat      cmdStat;
    };
};

#ifdef WIN32
#pragma warning(disable:4996)
#endif

#define OBJECT(name, type) \
    GlobalObjectManager::getInstance ()->getObject<type*>(name)

#ifdef WIN32
#define MOD_ENTRY extern "C" __declspec(dllexport)
#else
#define MOD_ENTRY extern "C"
#endif

#define DECLARE_NOODLE_PLUGIN(name) \
    MOD_ENTRY \
    Plugin* \
    hookProc () { \
        return new name; \
    }

#endif // noodle_hpp
