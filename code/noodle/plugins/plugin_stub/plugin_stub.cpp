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

#include <sstream>

#include "plugin_stub.hpp"

DECLARE_NOODLE_PLUGIN (PluginStub);

int PluginStub::getMessageInternal () {
    return OK;
}

int PluginStub::execMessageInternal (void *message_) {
    return OK;
}

int PluginStub::initPlugin (NE* environment_, const char* pluginName_) {
    PluginSource::initPlugin (environment_, pluginName_);

    _pluginName = pluginName_;

    std::stringstream queryIP;
    queryIP << "stub." << _pluginName << ".ip";

    std::stringstream queryPort;
    queryPort << "stub." << _pluginName << ".port";

    const char* ip = environment_->getString  (queryIP.str().c_str());
    const int port = environment_->getInteger (queryPort.str().c_str());
    //const int backLog = environment_->getInteger ("stub.backLog"); // reserved

    _server = new Server (environment_, this, ip, (short)port);
    _thread = new boost::thread (boost::bind (&PluginStub::routine, this));

    return OK;
}

int PluginStub::deinitPlugin (NE* environment_) {
    _server->stop ();

    boost::mutex::scoped_lock lock (_quitMutex);
    while (!_quit) {
        _quitCondition.wait (lock);
    }

    delete _thread;
    delete _server;
    return OK;
}

void PluginStub::routine () {
    _server->start ();
    boost::mutex::scoped_lock lock (_quitMutex);
    _quit = true;
    _quitCondition.notify_all ();
}
