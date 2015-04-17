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
 \file engine.hpp
 \brief noodle engine
 */

#ifndef engine_hpp
#define engine_hpp

#include <list>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>

#include "message_queue.hpp"
#include "message_source_manager.hpp"
#include "proxy_manager.hpp"

class AutoProxyTable;

typedef volatile unsigned long long Counter; ///< large integer counter type

/*!
 \brief noodle engine
 */

class Engine {
public:

    /*!
     \brief constructor
     \param proxyManager_ proxy manager
     \param messageSourceManager_ message source manager
     \param worker_ worker number in engine
     */

    Engine (ProxyManager* proxyManager_,
        MessageSourceManager* messageSourceManager_, int worker_ = 1);

    /*!
     \brief destructor
     */

    ~Engine ();

    /*!
     \brief start noodle engine
     \return OK or error code, get the error detail by ErrorString::errorString
     */

    int start ();

    /*!
     \brief halt noodle engine
     \return OK or error code, get the error detail by ErrorString::errorString
     */

    int halt ();

    /*!
     \brief get total message count
     \return total message count
     */

    inline Counter getTotalMessage () {
        return _totalMessage;
    }

    /*!
     \brief get normal message count
     \return normal message count
     */

    inline Counter getNormalMessage () {
        return _normalMessage;
    }

    /*!
     \brief get fail message count
     \return fail message count
     */

    inline Counter getFailedMessage () {
        return _failedMessage;
    }

    /*!
     \brief get except message count
     \return except message count
     */

    inline Counter getExceptMessage () {
        return _exceptMessage;
    }

    /*!
     \brief get worker count
     \return worker count
     */

    inline int getWorkerCount () {
        return (int)_workerList.size ();
    }

public:
    struct Worker {
        MessageQueue*         messageQueue;
        boost::thread*        thread;
        ProxyManager*         proxyManager;
        MessageSourceManager* messageSourceManager;
        Engine*               engine;
    };

private:
    typedef std::list<Worker*> WorkerList;

    bool                  _started;              ///< is engine started?
    WorkerList            _workerList;           ///< worker list
    boost::mutex          _quitMutex;            ///< the mutex for quit flag
    boost::condition      _quitCondition;        ///< the condition for quit flag
    ProxyManager*         _proxyManager;         ///< proxy manager
    MessageSourceManager* _messageSourceManager; ///< message source manager

public:
    Counter _totalMessage;   ///< total message count
    Counter _normalMessage;  ///< normal message count
    Counter _failedMessage;  ///< failed message count
    Counter _exceptMessage;  ///< exception message count
};

#endif // engine_hpp
