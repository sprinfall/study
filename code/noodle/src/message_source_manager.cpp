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
#include <algorithm>
#include <boost/bind.hpp>

#include "noodle.hpp"
#include "memory_pool.hpp"
#include "global_object_manager.hpp"
#include "proxy_manager.hpp"
#include "log.hpp"
#include "os_misc.hpp"
#include "message_source_manager.hpp"

void _sourceRoutine (MessageSourceManager::SourceParam* sourceParam_);

MessageSourceManager::~MessageSourceManager ()
{
    scope_write_lock (_rwmutex);
    MessageSourceTable::iterator mstIt = _messageSourceTable.begin ();
    for (; mstIt != _messageSourceTable.end (); mstIt++) {
        Message message;
        memset (&message, 0, sizeof (Message));
        message.messageID = MSG_QUIT;
        mstIt->second.proxy->putMessage (&message);
        ThreadTable::iterator ttIt = _threadTable.find (
            mstIt->second.proxy->getInstanceID ());
        ttIt->second->join ();
        _threadGroup.remove_thread (ttIt->second);
        delete ttIt->second;
        _threadTable.erase (ttIt);
    }
    _messageSourceTable.clear ();

    // cleanup all dirty messages

    Message* dirtyMessage = 0;
    while (dirtyMessage = _messageQueue->tryGetMessage ()) {
        MemoryPool* memoryPool =
            GlobalObjectManager::getInstance ()->getObject<MemoryPool*>(
                "memoryPool");
        if (dirtyMessage->body) {
            memoryPool->free (dirtyMessage->bodyLength, dirtyMessage->body);
        }
        memoryPool->free (sizeof (Message), dirtyMessage);
    }
}

int MessageSourceManager::registerMessageSource (Proxy* proxy_) {
    assert (proxy_);
    int instanceID = proxy_->getInstanceID ();
    scope_write_lock (_rwmutex);
    if (_messageSourceTable.end () != _messageSourceTable.find (instanceID)) {
        return ERR_DUP_PROXY;
    }    
    SourceParam param = {proxy_, _messageQueue};
    _messageSourceTable[instanceID] = param;
    boost::thread* thread = new boost::thread (
        boost::bind(
            _sourceRoutine,
            &_messageSourceTable[instanceID]));
    _threadGroup.add_thread (thread);
    _threadTable[instanceID] = thread;
    return OK;
}

int MessageSourceManager::unregisterMessageSource (int id_) {
    assert (id_);
    scope_write_lock (_rwmutex);
    MessageSourceTable::iterator mstIt =
        _messageSourceTable.find (id_);
    if (_messageSourceTable.end () == mstIt) {
        return ERR_UNKNOWN_PROXY;
    }

    DECL_MSG (message, MSG_QUIT);
    mstIt->second.proxy->putMessage (&message);

    ThreadTable::iterator ttIt = _threadTable.find (id_);
    ttIt->second->join ();
    _threadGroup.remove_thread (ttIt->second);
    _threadTable.erase (ttIt);
    _messageSourceTable.erase (mstIt);
    return OK;
}

int MessageSourceManager::unregisterMessageSource (
    const std::string& pluginName_) {
    scope_read_lock (_rwmutex);

    MessageSourceTable::iterator mstIt = _messageSourceTable.begin ();

    for (; mstIt != _messageSourceTable.end ();) {
        if (pluginName_ == mstIt->second.proxy->getPluginName ()) {
            DECL_MSG (message, MSG_QUIT);
            mstIt->second.proxy->putMessage (&message);

            ThreadTable::iterator ttIt = _threadTable.find (mstIt->first);
            ttIt->second->join ();
            _threadGroup.remove_thread (ttIt->second);
            _threadTable.erase (ttIt);
            _messageSourceTable.erase (mstIt++);
        } else {
            mstIt++;
        }
    }

    return OK;
}

bool MessageSourceManager::sourcable (int id_) {
    assert (id_);
    scope_read_lock (_rwmutex);
    MessageSourceTable::iterator mstIt =
        _messageSourceTable.find (id_);
    if (_messageSourceTable.end () == mstIt) {
        return false;
    }
    return true;
}

Message* MessageSourceManager::getMessage () {
    return _messageQueue->getMessage ();
}

int MessageSourceManager::putMessage (Message* message_) {
    assert (message_);
    scope_read_lock (_rwmutex);
    MessageSourceTable::iterator mstIt =
        _messageSourceTable.find (message_->nextID);
    if (_messageSourceTable.end () == mstIt) {
        return ERR_UNKNOWN_PROXY;
    }
    return mstIt->second.proxy->putMessage (message_);
}

Proxy* MessageSourceManager::getProxy (int id_) {
    MessageSourceTable::iterator mstIt = _messageSourceTable.find (id_);
    if (mstIt == _messageSourceTable.end ()) {
        return 0;
    }
    return mstIt->second.proxy;
}

void _sourceRoutine (MessageSourceManager::SourceParam* sourceParam_) {
    assert (sourceParam_);
    Log* log = OBJECT ("log", Log);

    for (;;) {

        Message* message = sourceParam_->proxy->getMessage ();

        if (!message) {
            log->writeErrorLog (
                "%s:%d",
                "get a null message, force to sleep 100ms",
                sourceParam_->proxy->getInstanceID ());

            // force thread to sleep 100ms, the source has been
            // disabled or inactived

            OS::sleep (100);
            continue;
        }

        if (MSG_QUIT == message->messageID) {
            log->writeNoticeLog (
                "%s:[%d]",
                "message source routine terminated",
                OS::gettid ());
            break;
        }

        sourceParam_->messageQueue->putMessage (message);
    }
}
