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

#include "test_proxy_stub_src.hpp"
#include "noodle.hpp"

DECLARE_NOODLE_PLUGIN (TestProxyStubSrc);

int TestProxyStubSrc::initPlugin (NE *environment_, const char* pluginName_) {
    PluginSource::initPlugin (environment_, pluginName_);
    _environment = environment_;
    _pluginName = pluginName_;

    return OK;
}

int TestProxyStubSrc::deinitPlugin (NE *environment_) {
    return OK;
}

#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

int TestProxyStubSrc::getMessageInternal () {
#ifdef WIN32
    ::Sleep (1000);
#else
    ::usleep (1000000);
#endif

    void* message =
        _environment->createMessage (
            MSG_CALL,
            _environment->getInstanceIdentifier ("plugin5"),
            "hello",
            6);

    putMessage (message);

    return OK;
}

int TestProxyStubSrc::execMessageInternal (void *message_) {
    return OK;
}
