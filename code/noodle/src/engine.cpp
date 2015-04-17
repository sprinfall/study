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

#include <iostream>
#include <cassert>
#include <boost/bind.hpp>
#include <stdexcept>
#include <string>
#include <fstream>

#include "noodle.hpp"
#include "memory_pool.hpp"
#include "global_object_manager.hpp"
#include "engine.hpp"
#include "log.hpp"
#include "os_misc.hpp"
#include "auto_proxy_table.hpp"
#include "module_manager.hpp"
#include "error_string.hpp"
#include "message_source_manager.hpp"
#include "proxy_manager.hpp"
#include "config_manager.hpp"
#include "version.hpp"

static void _workerRoutine (Engine::Worker* worker_);

Engine::Engine (ProxyManager* proxyManager_,
    MessageSourceManager* messageSourceManager_, int worker_)
        : _proxyManager (proxyManager_),
          _messageSourceManager (messageSourceManager_),
          _started (true),
          _totalMessage (0),
          _normalMessage (0),
          _failedMessage (0),
          _exceptMessage (0) {

    assert (_proxyManager);

    // create message queue for each worker
    // create workers
    // start workers

    for (int i = 0; i < worker_; i++) {
        Worker* worker = new Worker;
        worker->messageQueue = new MessageQueue ();
        worker->proxyManager = proxyManager_;
        worker->messageSourceManager = messageSourceManager_;
        worker->thread = new boost::thread (boost::bind(_workerRoutine, worker));
        worker->engine = this;
        _workerList.push_back (worker);
    }
}

Engine::~Engine () {

    // if there are still have workers in engine, terminate them

    if (!_workerList.empty ()) {
        halt ();
    }
}

int Engine::start () {

    int error = OK;

    WorkerList::iterator wlIt = _workerList.begin ();
    if (wlIt == _workerList.end ()) {
        throw std::runtime_error ("no worker in engine");
    }

    Log* log = GlobalObjectManager::getInstance ()->getObject<Log*>("log");

    for (; _started; wlIt++) {
        if (wlIt == _workerList.end ()) {
            wlIt = _workerList.begin ();
        }

        Message* message = _messageSourceManager->getMessage ();

        if (!message) {
            log->writeNoticeLog ("%s:[%d]", "get NULL message",
                OS::gettid ());
            continue;
        }

        // increase total message count

        _totalMessage++;

        if (MSG_QUIT == message->messageID) {

            // increase normal message count

            _normalMessage++;

            log->writeNoticeLog ("%s:[%d]", "engine routine terminated",
                OS::gettid ());
            break;
        }

        if (OK != (error = (*wlIt)->messageQueue->putMessage (message))) {

            // increase failed message count

            _failedMessage++;

            log->writeErrorLog ("error when queue message");
        } else {

            // increase normal message count

            _normalMessage++;
        }
    }

    // notify the caller of Engine::halt ()

    boost::mutex::scoped_lock lock (_quitMutex);
    _started = false;
    _quitCondition.notify_all ();

    return OK;
}

int Engine::halt () {

    DECL_MSG(message, MSG_QUIT);
    _messageSourceManager->getSourceQueue ()->putMessage (&message, true);

    // wait for engine's main thread joined

    boost::mutex::scoped_lock lock (_quitMutex);
    while (_started) {
        _quitCondition.wait (lock);
    }

    WorkerList::iterator wlIt = _workerList.begin ();
    for (; wlIt != _workerList.end (); wlIt++) {
        (*wlIt)->messageQueue->putMessage (&message, true);
        (*wlIt)->thread->join ();        

        // cleanup all dirty messages

        Message* dirtyMessage = 0;
        while (dirtyMessage = (*wlIt)->messageQueue->tryGetMessage ()) {
            MemoryPool* memoryPool =
                GlobalObjectManager::getInstance ()->getObject<MemoryPool*>("memoryPool");
            if (dirtyMessage->body) {
                memoryPool->free (dirtyMessage->bodyLength, dirtyMessage->body);
            }
            memoryPool->free (sizeof (Message), dirtyMessage);
        }

        // delete message queue, boost::thread and worker parameter

        delete (*wlIt)->messageQueue;
        delete (*wlIt)->thread;
        delete *wlIt;
    }

    _workerList.clear ();
    return OK;
}

#define FREE_MSG(p, m) \
    if (m->body) { \
        p->free (m->bodyLength, m->body); \
    } \
    p->free (sizeof (Message), m);

static void _workerRoutine (Engine::Worker* worker_) {

    assert (worker_);

    int error = OK;

    Log*                  log                  = OBJECT("log", Log);
    MemoryPool*           memoryPool           = OBJECT("memoryPool", MemoryPool);
    ModuleManager*        moduleManager        = OBJECT("moduleManager", ModuleManager);
    MessageSourceManager* messageSourceManager = OBJECT("messageSourceManager", MessageSourceManager);

    AutoProxyTable proxyTable (worker_->proxyManager);

    for (;;) {
        Message* message = worker_->messageQueue->getMessage ();

        if (!message) {
            log->writeNoticeLog ("%s:[%d]", "get NULL message", OS::gettid ());
            continue;
        }

        // need to quit worker routine

        if (MSG_QUIT == message->messageID) {
            log->writeNoticeLog ("%s:[%d]", "engine worker routine terminated", OS::gettid ());
            break;
        }

        // execute command from console

        if (MSG_CMD_MAX > message->messageID) {
            switch (message->messageID) {
                case MSG_DESTROY: {

                    Command* command = (Command*)(message->body);
                    int instanceID = command->cmdBasic.instanceID;

                    // unregister source proxy
                    
                    if (messageSourceManager->sourcable (instanceID)) {
                        Proxy* proxy = messageSourceManager->getProxy (instanceID);
                        messageSourceManager->unregisterMessageSource (instanceID);

                        // delete the proxy from proxy manager

                        worker_->proxyManager->destroyProxy (proxy);

                        // delete proxy from proxy manager

                        if (proxyTable.has (instanceID)) {
                            proxyTable.destroyProxy (instanceID);
                        }
                    }

                    // destroy instance which proxy observes to

                    error = moduleManager->destroyInstance (instanceID);
                    memcpy (message->body, ErrorString::errorString (error),
                        strlen (ErrorString::errorString (error)) + 1);

                    break;
                }
                case MSG_CREATE: {

                    Command* command = (Command*)(message->body);
                    int instanceID = command->cmdStart.instanceID;

                    std::string name (command->cmdStart.pluginName);

                    error = moduleManager->createInstance (name, instanceID);
                    memcpy (message->body, ErrorString::errorString (error),
                        strlen (ErrorString::errorString (error)) + 1);

                    // create source routine for sourcable instance

                    if (OK == error) {
                        if (moduleManager->sourcable (name)) {
                            Proxy* proxy = worker_->proxyManager->createProxy (instanceID);
                            messageSourceManager->registerMessageSource (proxy);
                        }
                    }
                    break;
                }
                case MSG_ENABLE: {

                    Command* command = (Command*)(message->body);
                    int instanceID = command->cmdBasic.instanceID;

                    error = moduleManager->enableInstance (instanceID);
                    memcpy (message->body, ErrorString::errorString (error),
                        strlen (ErrorString::errorString (error)) + 1);
                    break;
                }
                case MSG_DISABLE: {

                    Command* command = (Command*)(message->body);
                    int instanceID = command->cmdBasic.instanceID;

                    error = moduleManager->disableInstance (instanceID);
                    memcpy (message->body, ErrorString::errorString (error),
                        strlen (ErrorString::errorString (error)) + 1);
                    break;
                }
                case MSG_INSTALL: {

                    Command* command = (Command*)(message->body);
                    std::string pluginName = command->cmdInstall.pluginName;
                    std::string pluginPath = command->cmdInstall.pluginPath;

                    error = moduleManager->installModule (pluginName,
                        pluginPath, command->cmdInstall.source);

                    memcpy (message->body, ErrorString::errorString (error),
                        strlen (ErrorString::errorString (error)) + 1);
                    break;
                }
                case MSG_UNINSTALL: {

                    Command* command = (Command*)(message->body);
                    std::string pluginName = command->cmdUninstall.pluginName;

                    try {
                        if (moduleManager->sourcable (pluginName)) {

                            // unlink source proxy from message source manager

                            messageSourceManager->unregisterMessageSource (pluginName);

                            // destroy source proxy

                            worker_->proxyManager->destroySourceProxy (pluginName);
                        }
                    } catch (std::exception& e) {
                        memcpy (message->body, e.what (),
                            strlen (e.what () + 1));
                        break;
                    }

                    // destroy proxy from auto proxy table

                    proxyTable.destroyProxy (pluginName);

                    // need send to plugin console

                    error = moduleManager->uninstallModule (pluginName);

                    memcpy (message->body, ErrorString::errorString (error),
                        strlen (ErrorString::errorString (error)) + 1);

                    break;
                }
                case MSG_SHUTDOWN: {

                    static const char* shutdownMsg = "system is going to shutdown...";
                    memcpy (message->body, shutdownMsg, strlen (shutdownMsg) + 1);

                    // TODO: shutdown the system
                    // maybe donot support to shutdown system by console for security issues

                    break;
                }
                case MSG_UPDATE: {
                    Command* command = (Command*)(message->body);

                    std::string oldPluginName (command->cmdUpdate.oldPluginName);
                    std::string newPluginName (command->cmdUpdate.newPluginName);
                    std::string path (command->cmdUpdate.pluginPath);

                    try {
                        error = moduleManager->updateModule (oldPluginName,
                            newPluginName, path);
                    } catch (std::exception& e) {

                        // need send to plugin console

                        memcpy (message->body, e.what (),
                            strlen (e.what () + 1));
                        break;
                    }

                    // need send to plugin console

                    memcpy (message->body, ErrorString::errorString (error),
                        strlen (ErrorString::errorString (error)) + 1);

                    break;
                }
                case MSG_STAT: {
                    Command* command = (Command*)(message->body);

                    if (command->cmdStat.printTree) {
                        std::stringstream ostream;
                        moduleManager->printTree (ostream);
                        memcpy (message->body, ostream.str ().c_str (),
                            ostream.str ().size () + 1);
                    } else if (command->cmdStat.printConfig) {
                        ConfigManager* configManager = OBJECT("config", ConfigManager);

                        std::string path (configManager->getPath ());
                        std::ifstream istream (path.c_str (), std::ios::in);
                        std::stringstream sstream;
                        sstream << istream.rdbuf();

                        if (message->bodyLength < (int)(sstream.str ().size () + 1)) {
                            memcpy (message->body, "config file size is too large",
                                strlen ("config file size is too large") + 1);
                        } else {
                            memcpy (message->body, sstream.str ().c_str (),
                                sstream.str ().size () + 1);
                        }
                    } else if (command->cmdStat.printMessage) {
                        std::stringstream sstream;
                        sstream << "total message: " << worker_->engine->getTotalMessage () << "\n";
                        sstream << "normal message:" << worker_->engine->getNormalMessage () << "\n";
                        sstream << "failed message:" << worker_->engine->getFailedMessage () << "\n";
                        sstream << "except message:" << worker_->engine->getExceptMessage ();

                        memcpy (message->body, sstream.str ().c_str (),
                            sstream.str ().size () + 1);
                    } else if (command->cmdStat.printVersion) {
                        std::stringstream sstream;
                        sstream << "core:           " << CORE_VERSION << "\n";
                        sstream << "engine:         " << ENGINE_VERSION << "\n";              
                        sstream << "logger:         " << LOG_VERSION << "\n";              
                        sstream << "queue:          " << MESSAGE_QUEUE_VERSION << "\n";  
                        sstream << "config manager: " << CONFIG_MANAGER_VERSION << "\n";
                        sstream << "module manager: " << MODULE_MANAGER_VERSION << "\n";
                        sstream << "proxy manager:  " << PROXY_MANAGER_VERSION << "\n";
                        sstream << "source manager: " << MESSAGE_SOURCE_MANAGER_VERSION << "\n";
                        sstream << "script:         " << MINI_SCRIPT_VERSION;

                        memcpy (message->body, sstream.str ().c_str (),
                            sstream.str ().size () + 1);
                    } else if (command->cmdStat.printPool) {
                        std::stringstream sstream;
                        sstream << "pool size:       " << memoryPool->getSize () << "\n";
                        sstream << "pool alloc size: " << memoryPool->getActual () << "\n";
                        sstream << "pool max size:   " << memoryPool->getMax ();

                        memcpy (message->body, sstream.str ().c_str (),
                            sstream.str ().size () + 1);
                    }

                    break;
                }
                default:
                    break;
            }
        }

        // dispatch message to plugin

        do {
            try {
                if (OK != (error = proxyTable[message->nextID]->putMessage (message))) {
                    if (DONE == error) {
                        ;
                    } else {
                        worker_->engine->_failedMessage++;

                        if ((STATE_INACTIVE != proxyTable[message->nextID]->getState ()) &&
                            (STATE_DISABLE  != proxyTable[message->nextID]->getState ())) {
                            log->writeErrorLog ("error when execute message:%s:[%d]",
                                    ErrorString::errorString (error), message->nextID );
                        } else {
                            log->writeErrorLog ("error when execute message:%s:[%d]",
                                ErrorString::errorString (error), message->nextID );
                        }
                    }
                }
            } catch (std::exception& e) {
                worker_->engine->_exceptMessage++;
                log->writeErrorLog ("exception:%s:[%d]", e.what (), message->nextID);
                break;
            }
        } while (OK == error);

        // free unused message

        FREE_MSG (memoryPool, message);
    }
}
