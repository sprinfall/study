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
 \file message_source_manager.hpp
 \brief message source manager
 */

#ifndef message_source_manager_hpp
#define message_source_manager_hpp

#include <map>
#include <boost/thread.hpp>
#include "read_write_lock.hpp"
#include "message_queue.hpp"
#include "proxy.hpp"

/*!
 \brief message source manager
 */

class MessageSourceManager {
public:

    /*!
     \brief constructor
     */

#if (BOOST_VERSION < 103500)
    MessageSourceManager (MessageQueue* messageQueue_)
        : _messageQueue (messageQueue_),
        _rwmutex (boost::read_write_scheduling_policy::writer_priority) {}
#else
    MessageSourceManager (MessageQueue* messageQueue_)
        : _messageQueue (messageQueue_) {}
#endif

    /*!
     \brief destructor
     */

    ~MessageSourceManager ();

    /*!
     \brief register a source proxy
     \param proxy_ source proxy
     \return ok or error code
     */

    int registerMessageSource (Proxy* proxy_);

    /*!
     \brief unregister a source proxy
     \param id_ the instance ID of proxy
     \return ok or error code
     */

    int unregisterMessageSource (int id_);

    /*!
     \brief unregister source proxy by plugin name
     \param pluginName_ plugin name
     \return ok or error code
     */

    int unregisterMessageSource (const std::string& pluginName_);

    /*!
     \brief get message from source proxy
     \return message pointer
     */

    Message* getMessage ();

    /*!
     \brief get message from source proxy
     \return message pointer
     */

    int putMessage (Message* message_);

    /*!
     \brief get message queue of message source manager
     \return message queue pointer
     */

    inline MessageQueue* getSourceQueue () {
        return _messageQueue;
    }

    /*!
     \brief get proxy by instance ID
     \return proxy pointer
     */

    Proxy* getProxy (int id_);

    /*!
     \brief check whether has the source proxy forked from specific instance
     \return true of false
     */

    bool sourcable (int id_);

public:
    struct SourceParam {
        Proxy* proxy;
        MessageQueue* messageQueue;
    };

private:
    typedef std::map<int, SourceParam>    MessageSourceTable;
    typedef std::map<int, boost::thread*> ThreadTable;
    typedef std::list<boost::thread*>     ThreadList;

#if (BOOST_VERSION >= 103500)
    boost::shared_mutex     _rwmutex; ///< read/write mutex
#else
    boost::read_write_mutex _rwmutex; ///< read/write mutex
#endif

    ThreadTable             _threadTable;        ///< thread table
    MessageSourceTable      _messageSourceTable; ///< message source table
    MessageQueue*           _messageQueue;       ///< message queue
    boost::thread_group     _threadGroup;        ///< boost thread group
};

#endif // message_source_manager_hpp
