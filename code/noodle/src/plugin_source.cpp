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
 \file plugin_source.cpp
 \brief implement source plugin base class
 */

#include "noodle.hpp"
#include "plugin_source.hpp"

int PluginSource::initPlugin (NE *environment_, const char* pluginName_) {
    _environment = environment_;
    return OK;
}

int PluginSource::deinitPlugin (NE *environment_) {
    void* message = 0;

    // clear all dirty messages in message queue

    while (message = _messageQueue.tryGetMessage ()) {
        _environment->destroyMessage (message);
    }
    return OK;
}

int PluginSource::initInstance (int id_) {
    return OK;
}

int PluginSource::deinitInstance (int id_) {
    return OK;
}

void* PluginSource::getMessage () {
    void* message = 0;

    // 1. try to get message from message queue first
    // 2. call PluginSource::getMessageInternal ()
    // 3. plugin source should return OK when new message produced

    for (;;) {
        if (message = _messageQueue.tryGetMessage ()) {
            break;
        } else {
            if (OK == getMessageInternal ()) {
                return _messageQueue.getMessage ();
            }
        }
    }
    return message;
}

int PluginSource::putMessage (void *message_, bool urgent_) {

    // put a message to message queue

    return _messageQueue.putMessage (message_, urgent_);
}

int PluginSource::execMessage (void *message_) {

    // 1. resend MSG_QUIT to noodle engine
    // 2. call PluginSource::execMessageInternal to execute message

    switch (_environment->getAction (message_)) {
        case MSG_QUIT:
            putMessage (message_, true);
            return OK;
        default:
            break;
    }
    return execMessageInternal (message_);
}
