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
 \file core.hpp
 \brief the entrance to start noodle engine
 */

#ifndef core_hpp
#define core_hpp

#include "log.hpp"
#include "engine.hpp"
#include "message_queue.hpp"
#include "module_manager.hpp"
#include "proxy_manager.hpp"
#include "memory_pool.hpp"
#include "message_source_manager.hpp"
#include "global_object_manager.hpp"
#include "config_manager.hpp"
#include "ne_impl.hpp"

class Core {
public:
    /*!
     \brief constructor
     */

    Core ()
        : _engine               (0),
          _log                  (0),
          _coreMessageQueue     (0),
          _configManager        (0),
          _moduleManager        (0),
          _proxyManager         (0),
          _messageSourceManager (0),
          _memoryPool           (0),
          _logLevel             (0),
          _daemon               (false),
          _noodleEnvironment    (0),
          _terminated           (true),
          _logFlush             (false),
#ifndef WIN32
          _pending              (false),
          _restart              (false),
          _reload               (false),
          _pendingThread        (0),
#endif // !WIN32
          _argc                 (0),
          _argv                 (0) {}

    /*!
     \brief destructor

            cleanup all resources
     */

    ~Core ();

    /*!
     \brief start core
     \param argc_ the word count of command line
     \param argv_ the string array of command line
     \return ture of false
     */

    bool start (int argc_, char** argv_);

    /*!
     \brief restart core
     \return true of false
     */

    bool restart ();

    /*!
     \brief halt core
     \return true of false
     */

    bool halt ();

    /*!
     \brief halt core without waiting
     \return true of false
     */

    bool haltNoWait ();

#ifndef WIN32

    /*!
     \brief set pending flag
     */

    inline void postPending () {
        _pending = true;
    }

    /*!
     \brief set restart flag
     */

    inline void postRestart () {
        _restart = true;
    }

    /*!
     \brief set reload config file flag
     */

    inline void postReloadConfig () {
        _reload = true;
    }

#endif // !WIN32

private:
    
    /*!
     \brief the real method to start core, wrapped by start
     \param argc_ the word count of command line
     \param argv_ the string array of command line
     \return ture of false
     */

    bool _start (int argc_, char** argv);

    /*!
     \brief create PID file
     \return true of false
     */

    bool _createPID ();
    
    /*!
     \brief remove PID file
     \return true or false
     */

    bool _removePID ();

    /*!
     \brief get PID from PID file
     \return PID > 0, 0 or -1
     */

    int _getPID ();

#ifndef WIN32

    /*!
     \brief pending thread routine
     */

    void _pendingThreadRoutine ();

    /*!
     \brief clean up pending thread when halt or restart
     */

    inline void _cleanup () {
        if (_pending) {
            _pendingThread->join ();
            delete _pendingThread;
            _pending = false;
            _pendingThread = 0;
        }
    }

#endif // !WIN32

private:
    Engine*               _engine;               ///< noodle engine
    Log*                  _log;                  ///< logger
    MessageQueue*         _coreMessageQueue;     ///< the message queue of core
    ConfigManager*        _configManager;        ///< config manager
    ModuleManager*        _moduleManager;        ///< module manager 
    ProxyManager*         _proxyManager;         ///< proxy manager
    MemoryPool*           _memoryPool;           ///< memory pool
    MessageSourceManager* _messageSourceManager; ///< message source manager
    NE*                   _noodleEnvironment;    ///< noodle environment

    int                   _argc;       ///< the number of parameter
    char**                _argv;       ///< the parameter list
    bool                  _terminated; ///< whether core has been terminated
    bool                  _daemon;     ///< to be a daemon?
    std::string           _pidFile;    ///< pid file path
    std::string           _logFile;    ///< log file path
    int                   _logLevel;   ///< log level
    bool                  _logFlush;   ///< whether flush the I/O buffer for each writing
    std::string           _configFile; ///< config file path
#ifndef WIN32
    bool                  _pending;       ///< pending flag
    bool                  _restart;       ///< restart flag
    bool                  _reload;        ///< reload flag
    boost::thread*        _pendingThread; ///< pending thread
#endif // !WIN32
    boost::mutex          _mutex;
    boost::condition      _condition;
};

#endif // core_hpp
