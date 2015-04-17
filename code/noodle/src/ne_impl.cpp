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

#include <cstring>
#include <cassert>
#include <cstdarg>

#include "message.hpp"
#include "ne_impl.hpp"

#define GET_OBJECT(name, type) \
    GlobalObjectManager::getInstance()->getObject<type*>(name);

NEImpl::NEImpl ()
    : _log (0),
      _memoryPool (0),
      _configManager (0),
      _moduleManager (0) {

    _log              = GET_OBJECT ("log", Log);
    _memoryPool       = GET_OBJECT ("memoryPool", MemoryPool);
    _configManager    = GET_OBJECT ("config", ConfigManager);
    _moduleManager    = GET_OBJECT ("moduleManager", ModuleManager);
    _coreMessageQueue = GET_OBJECT ("coreMessageQueue", MessageQueue);
}

void* NEImpl::createMessage (int action_, int station_,
    const void* attachment_, int length_) {

    Message* message =
        static_cast<Message*>(_memoryPool->alloc (sizeof (Message)));

    if (!message) {
        return 0;
    }

    memset (message, 0, sizeof (Message));
    message->messageID  = action_;
    message->bodyLength = length_;
    message->nextID     = station_;

    if (length_) {
        if (attachment_) {
            message->body = _memoryPool->alloc (length_);
            if (!message->body) {
                _memoryPool->free (sizeof (Message), message);
                return 0;
            }
            memcpy (message->body, attachment_, length_);
        } else {
            message->body = _memoryPool->alloc (length_);
            if (!message->body) {
                _memoryPool->free (sizeof (Message), message);
                return 0;
            }
        }
    }

    return message;
}

int NEImpl::postMessage (void* message_) {
    return _coreMessageQueue->putMessage ((Message*)message_);
}

int NEImpl::getInstanceIdentifier (const char* pluginName_) {
    assert (pluginName_);
    // TODO: wait for instance create
    try {
        return _moduleManager->getRandomInstanceID (std::string (pluginName_));
    } catch (std::exception& e) {
        (void) e;
        return 0;
    }
}

void NEImpl::destroyMessage (void* message_) {
    assert (message_);

    Message* message = static_cast<Message*>(message_);
    if (message->body) {
        _memoryPool->free (message->bodyLength, message->body);
    }
    _memoryPool->free (sizeof (Message), message);
}

void NEImpl::setStation (void* message_, int station_) {
    assert (message_);
    assert (station_);
    static_cast<Message*>(message_)->nextID = station_;
}

int NEImpl::getAction (void* message_) {
    assert (message_);
    return static_cast<Message*>(message_)->messageID;
}

void NEImpl::setAction (void* message_, int action_) {
    assert (message_);
    assert (action_);
    static_cast<Message*>(message_)->messageID = action_;
}

void* NEImpl::getAttachment (void* message_) {
    assert (message_);
    return static_cast<Message*>(message_)->body;
}

void NEImpl::setAttachment (void* message_, void* attachment_,
    int length_) {
    assert (message_);
    assert (attachment_);

    Message* message = static_cast<Message*>(message_);

    if (message->body) {
        if (message->bodyLength != length_) {
            _memoryPool->free (message->bodyLength, message->body);
        } else {
            memcpy (message->body, attachment_, length_);
            return;
        }
    }

    message->body = _memoryPool->alloc (length_);
    memcpy (message->body, attachment_, length_);
}

int NEImpl::getAttachmentLength (void* message_) {
    assert (message_);
    return static_cast<Message*>(message_)->bodyLength;
}

void* NEImpl::alloc (int size_) {
    return _memoryPool->alloc (size_);
}

void NEImpl::free (void* m_, int size_) {
    _memoryPool->free (size_, m_);
}

const int NEImpl::getInteger (const char* query_) {
    assert (query_);
    return _configManager->getInteger (query_);
}

const double NEImpl::getDouble (const char* query_) {
    assert (query_);
    return _configManager->getDouble (query_);
}

const char* NEImpl::getString (const char* query_) {
    assert (query_);
    return _configManager->getString (query_);
}

const bool NEImpl::getBoolean (const char* query_) {
    assert (query_);
    return _configManager->getBoolean (query_);
}

const int NEImpl::getArrayLength (const char* query_) {
    assert (query_);
    return _configManager->getArrayLength (query_);
}

const int NEImpl::getArrayIntegerByIndex (const char* query_, int index_) {
    assert (query_);
    return _configManager->getArrayIntegerByIndex (query_, index_);
}

const double NEImpl::getArrayDoubleByIndex (const char* query_, int index_) {
    assert (query_);
    return _configManager->getArrayDoubleByIndex (query_, index_);
}

const char* NEImpl::getArrayStringByIndex (const char* query_, int index_) {
    assert (query_);
    return _configManager->getArrayStringByIndex (query_, index_);
}

const bool NEImpl::getArrayBooleanByIndex (const char* query_, int index_) {
    assert (query_);
    return _configManager->getArrayBooleanByIndex (query_, index_);
}

const int NEImpl::getDictLength (const char* query_) {
    assert (query_);
    return _configManager->getDictLength (query_);
}

const int NEImpl::getDictIntegerKeyByIndex (const char* query_, int index_) {
    assert (query_);
    return _configManager->getDictIntegerKeyByIndex (query_, index_);
}

const double NEImpl::getDictDoubleKeyByIndex (const char* query_, int index_) {
    assert (query_);
    return _configManager->getDictDoubleKeyByIndex (query_, index_);
}

const char* NEImpl::getDictStringKeyByIndex (const char* query_ ,int index_) {
    assert (query_);
    return _configManager->getDictStringKeyByIndex (query_, index_);
}

const bool NEImpl::getDictBooleanKeyByIndex (const char* query_, int index_) {
    assert (query_);
    return _configManager->getDictBooleanKeyByIndex (query_, index_);
}

void NEImpl::writeLog (int level_, const char* fmt_, ...){
    assert (fmt_);
    va_list vas;
    va_start (vas, fmt_);
    _log->writeLog (level_, fmt_, vas);
}

void NEImpl::writeLog (int level_, int prefix_, const char* fmt_, ...) {
    assert (fmt_);
    va_list vas;
    va_start (vas, fmt_);
    _log->writeLog (level_, prefix_, fmt_, vas);
}

void NEImpl::writeLog (int level_, int prefix_, const char* file_,
    const char* func_, int line_, const char* fmt_, ...) {
    assert (fmt_);
    assert (file_);
    assert (func_);
    va_list vas;
    va_start (vas, fmt_);
    _log->writeLog (level_, prefix_, file_, func_, line_, fmt_, vas);
}
