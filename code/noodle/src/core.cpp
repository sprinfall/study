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
#include <boost/program_options.hpp>
#include <boost/bind.hpp>
#include <string>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <cerrno>
#include <cstring>
#include <sstream>

#ifndef WIN32
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#endif

#include "os_misc.hpp"
#include "core.hpp"

using namespace boost::program_options;

Core::~Core () {

    // terminate noodle core if need to do that

    if (!_terminated) {
        halt ();
    }

#ifndef WIN32
    _cleanup ();
#endif
}

bool Core::start (int argc_, char** argv_) {

    // already started!

    if (!_terminated) {
        return false;
    }

    bool res = false;

    OS::setRandomSeed ((unsigned)time (0));

    // argc_ and argv_ should be never
    // deallocated before process exit

    _argc = argc_;
    _argv = argv_;

    // Core::start () already be called

    _terminated = false;

    // register to global object manager before call 'setCtrlHandler'

    GlobalObjectManager::getInstance ()->registerObject ("core", this);

#ifdef WIN32

    // set handler for software interruption

    OS::setCtrlHandler ();
#endif

    try {
        res = _start (argc_, argv_);

        // _start (..., ...) want to quit

        if (!res) {
            return res;
        }
    } catch (std::exception& e) {
        std::cout << e.what () << std::endl;
        return false;
    }

    // wait the notification from Core::halt (), Core::restart ()
    {
        boost::mutex::scoped_lock lock (_mutex);
        while (!_terminated) {
            _condition.wait (lock);
        }
    }

#ifndef WIN32
    if (_restart) {

         // clear attributes

        _engine               = 0;
        _log                  = 0;
        _coreMessageQueue     = 0;
        _configManager        = 0;
        _moduleManager        = 0;
        _proxyManager         = 0;
        _messageSourceManager = 0;
        _memoryPool           = 0;
        _logLevel             = 0;
        _daemon               = false;
        _noodleEnvironment    = 0;
        _logFlush             = false;
        _restart              = false;

        _pidFile.clear ();
        _logFile.clear ();
        _configFile.clear ();

        return start (_argc, _argv);
    }
#endif

    return res;
}

bool Core::restart () {
#ifndef WIN32

    if (!haltNoWait ()) {
        return false;
    }

    boost::mutex::scoped_lock lock (_mutex);
    _terminated = true;
    _condition.notify_all ();

#endif // !WIN32

    return true;
}

bool Core::haltNoWait () {

    // halt engine

    if (_engine) {
        _engine->halt ();
        delete _engine;
    }

    // halt message source manager

    if (_messageSourceManager) {
        delete _messageSourceManager;
    }

    // destroy proxy manager

    if (_proxyManager) {
        delete _proxyManager;
    }

    // destroy module manager

    if (_moduleManager) {
        delete _moduleManager;
    }

    // destroy core message queue

    if (_coreMessageQueue) {
        delete _coreMessageQueue;
    }

    // destroy config manager

    if (_configManager) {
        delete _configManager;
    }

    // destroy memory pool

    if (_memoryPool) {
        try {
            delete _memoryPool;
        } catch (std::exception& e) {

            // for some reason, the memory allocated in plugin,
            // but does not free when system terminating

            _log->writeErrorLog ("%s", e.what ());
        }
    }

    // destroy noodle environment

    if (_noodleEnvironment) {
        delete _noodleEnvironment;
    }

    // destroy log, must be the last one to destroy

    if (_log) {
        delete _log;
    }

    // unregister all global objects

    GlobalObjectManager::getInstance()->unregisterAll ();

   
    return true;
}

bool Core::halt () {    

    if (!haltNoWait ()) {
        return false;
    }

    // notify the thread which supend on Core::start ()

    boost::mutex::scoped_lock lock (_mutex);
    _terminated = true;
    _condition.notify_all ();

    return true;
}

bool Core::_start (int argc_, char** argv) {

    // TODO: add a mode to sperate log writing
    // TODO: add a plugin to sync log
    // TODO: set signal mask for thread

    options_description desc("Noodle engine options");

    desc.add_options()
    ("help,h",   "show help message")
    ("daemon,d", "started as a daemon")
    ("log,l",    value<std::string>(), "set log file path")
    ("level,e",  value<int>(),         "set log level")
    ("pid,p",    value<std::string>(), "set pid file path")
    ("config,c", value<std::string>(), "set config file path")
#ifndef WIN32
    ("server,s", value<std::string>(), "control engine lifecycle")
#endif
    ("mode,m",   value<bool>(),        "set log mode")
    ;

    // parse program options

    variables_map vm;
    try {
        store(parse_command_line(argc_, argv, desc), vm);
        notify(vm);
    } catch (std::exception& e) {
        std::cout << e.what () << std::endl;
        return false;
    }

    if (vm.count ("help")) {
        std::cout << desc << std::endl;
        return false;
    }

    if (vm.count ("config")) {
        _configFile = vm["config"].as<std::string>();
    }

    // setup config manager

    if (_configFile.size ()) {
        _configManager = new ConfigManager (_configFile.c_str ());
        if (!_configManager->readConfig ()) {
            return false;
        }
    } else {
        if (!OS::getEnv ("ne_config")) {
            std::cout << "cannot find 'ne_config' environment variable"
                << std::endl;
            return false;
        }

        _configFile = OS::getEnv ("ne_config");
        _configManager = new ConfigManager (_configFile.c_str ());
        if (!_configManager->readConfig ()) {
            return false;
        }
    }

    GlobalObjectManager::getInstance ()->registerObject ("config",
        _configManager);

    // get PID file path

    if (vm.count ("pid")) {
        _pidFile = vm["pid"].as<std::string>();
    }

    if (!_pidFile.size ()) {
        _pidFile = _configManager->getString ("ne.system.pid");
    }

#ifndef WIN32
    if (vm.count ("server")) {

        std::ifstream pidFile (_pidFile.c_str ());
        if (!pidFile) {
            std::cout << "PID file not found" << std::endl;
            return false;
        }

        int pid = 0;
        pidFile >> pid;

        // command start, restart, stop, reload

        std::string action = vm["server"].as<std::string>();
        std::cout << "Engine PID:" << pid << std::endl;
        if (action.size ()) {
            if ("start" == action) {
                ;
            } else {
                if ("restart" == action) {
                    kill (pid, SIGUSR1);
                } else if ("stop" == action) {
                    kill (pid, SIGTERM);
                } else if ("reload" == action) {
                    kill (pid, SIGUSR2);
                } else {
                    ;
                }

                // always return false to terminate Noodle

                return false;
            }
        }
    }
#endif

    // setup log

    if (vm.count ("log")) {
        _logFile = vm["log"].as<std::string>();
    }

    if (vm.count ("level")) {
        _logLevel = vm["level"].as<int>();
        if (_logLevel < Log::EMERG && _logLevel > Log::DEBUG) {
            std::cout << "invalid log level" << std::endl;
            return false;
        }
    }

    if (!_logFile.size ()) {
        try {
            _logFile = _configManager->getString ("ne.system.log.name");
        } catch (std::exception& e) {
            (void) e;
            std::cout << "need set 'ne.system.log.name'" << std::endl;
            return false;
        }
    }

    if (!_logLevel) {
        _logLevel = _configManager->getInteger ("ne.system.log.level");
        if (_logLevel < Log::EMERG && _logLevel > Log::DEBUG) {
            std::cout << "invalid log level" << std::endl;
            return false;
        }
    }

    try {
        _logFlush = _configManager->getBoolean ("ne.system.log.flush");
    } catch (std::exception& e) {
        (void) e;
        _logFlush = false;
    }

    _log = new Log (_logFile, _logLevel, _logFlush);

    GlobalObjectManager::getInstance ()->registerObject ("log", _log);

    // to be a daemon

    if (vm.count ("daemon")) {
        _daemon = true;        
    } else {
        _daemon = _configManager->getBoolean ("ne.system.daemon");
    }

    if (_daemon) {
        if (OS::daemon ()) {
            std::cout << strerror (errno) << std::endl;
            return false;
        }
    }

#ifndef WIN32

    // start pending thread if hasn't

    if (!_pendingThread) {
        _pendingThread = new boost::thread (
            boost::bind (&Core::_pendingThreadRoutine, this));

        // set handler for software interruption

        OS::setCtrlHandler ();
    }

#endif

    ////////////////////////////////////////////////
    // from now on, the output should be log file //
    ////////////////////////////////////////////////

    // write pid file

    std::ofstream pidFile (_pidFile.c_str ());
    if (!pidFile) {
        _log->writeErrorLog ("cannot open pid file %s", _pidFile.c_str ());
        return false;
    }
    pidFile << OS::getpid ();
    pidFile.close ();

    // core message queue

    _coreMessageQueue = new MessageQueue ();
    GlobalObjectManager::getInstance ()->registerObject ("coreMessageQueue",
        _coreMessageQueue);

    // memory pool, must be setup before message source manager

    int    poolMax    = _configManager->getInteger ("ne.system.pool.max");
    int    poolBegin  = _configManager->getInteger ("ne.system.pool.begin");
    int    poolEnd    = _configManager->getInteger ("ne.system.pool.end");
    int    poolGap    = _configManager->getInteger ("ne.system.pool.gap");
    bool   poolSingle = _configManager->getBoolean ("ne.system.pool.singleChunk");
    double poolFactor = _configManager->getDouble  ("ne.system.pool.factor");

    _memoryPool = new MemoryPool (poolMax, poolBegin, poolEnd, poolGap,
        poolSingle, (float)poolFactor);

    GlobalObjectManager::getInstance ()->registerObject ("memoryPool",
        _memoryPool);

    // message source manager
    // TODO: add a start method, make sure the source thread
    //       should start after all plugins loaded

    _messageSourceManager = new MessageSourceManager (_coreMessageQueue);

    GlobalObjectManager::getInstance ()->registerObject (
        "messageSourceManager", _messageSourceManager);

    // module manager

    _moduleManager = new ModuleManager ();

    GlobalObjectManager::getInstance ()->registerObject ("moduleManager",
        _moduleManager);

    // proxy manager

    _proxyManager = new ProxyManager (_moduleManager);

    GlobalObjectManager::getInstance ()->registerObject ("proxyManager",
        _proxyManager);

    // noodle environment

    _noodleEnvironment = new NEImpl ();

    GlobalObjectManager::getInstance ()->registerObject ("environment",
        _noodleEnvironment);

    //////////////////////////////////////////////////
    // from now on, the noodle environment is setup //
    //////////////////////////////////////////////////
  
    std::string pluginPath = OS::validatePath(
        std::string (_configManager->getString ("ne.plugin.path")));

    int pluginNumber = _configManager->getDictLength ("ne.plugin.plugins");

    for (int i = 0; i < pluginNumber; i++) {

        // get plugin name

        std::string pluginName =
            _configManager->getDictStringKeyByIndex ("ne.plugin.plugins", i);

        // get plugin SO name

        std::stringstream queryPluginName;
        queryPluginName << "ne.plugin.plugins" << "['" << pluginName
            << "'][0]";

        std::string pluginSO =
            _configManager->getString (queryPluginName.str ().c_str ());

        // get source flag

        std::stringstream queryPluginInstanceSourcable;
        queryPluginInstanceSourcable << "ne.plugin.plugins" << "['"
            << pluginName << "'][1]";
        
        bool sourcable =
            _configManager->getBoolean (
                queryPluginInstanceSourcable.str ().c_str ());

        // install SO

        int res = _moduleManager->installModule (pluginName,
            pluginPath + pluginSO, sourcable);
        if (OK != res) {
            _log->writeErrorLog ("%s %s%s", "cannot load plugin",
                pluginPath.c_str (), pluginSO.c_str ());
            return false;
        }

        // get instance number of SO

        std::stringstream queryPluginInstanceNumber;
        queryPluginInstanceNumber << "ne.plugin.plugins" << "['" << pluginName
            << "'][2]";
        int pluginInstanceNumber =
            _configManager->getArrayLength (
                queryPluginInstanceNumber.str ().c_str ());

        // create instance

        for (int j = 0; j < pluginInstanceNumber; j++) {
            std::stringstream queryPluginInstanceID;
            queryPluginInstanceID << "ne.plugin.plugins" << "['" << pluginName
                << "'][2][" << j << "]";

            int instanceID =
                _configManager->getInteger (
                    queryPluginInstanceID.str ().c_str ());

            res = _moduleManager->createInstance (pluginName, instanceID);
            if (OK != res) {
                _log->writeErrorLog ("%s %s%s",
                    "cannot create instance for plugin",
                    pluginPath.c_str (), pluginSO.c_str ());
                return false;
            }            

            // here, only create proxy for source plugin
            // for others, engine will create proxy for them

            if (sourcable) {
                Proxy* sourceProxy = 0;

                // create proxy for source plugin

                if (!(sourceProxy = _proxyManager->createProxy (instanceID))) {
                    _log->writeErrorLog ("%s %d %s%s",
                        "cannot create proxy for instance", instanceID,
                        pluginPath.c_str (), pluginSO.c_str ());
                    return false;
                }

                // register to message source manager

                if (OK != _messageSourceManager->registerMessageSource (
                        sourceProxy)) {
                    _log->writeErrorLog ("%s %d %s%s",
                        "cannot add proxy to message source manager",
                        instanceID, pluginPath.c_str (), pluginSO.c_str ());
                    return false;
                }
            }
        }
    }

    // engine

    int worker = _configManager->getInteger ("ne.system.engine.worker");
    _engine = new Engine (_proxyManager, _messageSourceManager, worker);

    GlobalObjectManager::getInstance ()->registerObject ("engine", _engine);

    _log->writeNoticeLog ("Noodle Engine started");

    return (OK == _engine->start () ? true : false);
}

#ifndef WIN32
    void Core::_pendingThreadRoutine () {

        // waiting for system signal

        while (!_pending) {
            if (_restart) {
                _log->writeNoticeLog ("Noodle Engine restarting");

                // call restart () to cleanup necessary resource
                // and reconstruct Noodle Engine

                restart ();
            } else if (_reload) {
                _reload = false;

                // TODO: not strong enough when failed

                _log->writeNoticeLog ("Config file Reloading");

                if (!_configManager->reloadConfig ()) {
                    _log->writeErrorLog ("Reload config file failed");
                }
            }

            OS::sleep (2000);
        }

        if (!_terminated) {
            halt ();
        }
    }
#endif
