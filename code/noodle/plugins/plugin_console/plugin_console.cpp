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

#include "plugin_console.hpp"

DECLARE_NOODLE_PLUGIN (PluginConsole);

int PluginConsole::getMessageInternal () {
    return OK;
}

int PluginConsole::execMessageInternal (void *message_) {
    _result = (char*)(getEnvironment ()->getAttachment (message_));

    boost::mutex::scoped_lock lock (_execMutex);
    _exec = true;
    _execCondition.notify_all ();

    return DONE;
}

void PluginConsole::wait () {
    boost::mutex::scoped_lock lock (_execMutex);
    _exec = false;
    while (!_exec) {
        _execCondition.wait (lock);
    }
}

std::string PluginConsole::getResult () {
    return _result;
}

int PluginConsole::initPlugin (NE* environment_, const char* pluginName_) {
    PluginSource::initPlugin (environment_, pluginName_);

    const char* ip = environment_->getString  ("console.ip");
    const int port = environment_->getInteger ("console.port");

    _server = new Server (environment_, this, ip, (short)port);
    _thread = new boost::thread (boost::bind (&PluginConsole::routine, this));

    getEnvironment ()->writeNoticeLog ("Console server started");

    return OK;
}

int PluginConsole::deinitPlugin (NE* environment_) {
    _server->stop ();

    boost::mutex::scoped_lock lock (_quitMutex);
    while (!_quit) {
        _quitCondition.wait (lock);
    }

    delete _thread;
    delete _server;
    return OK;
}

void PluginConsole::routine () {
    _server->start ();
    boost::mutex::scoped_lock lock (_quitMutex);
    _quit = true;
    _quitCondition.notify_all ();
}
