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

#ifndef plugin_proxy_hpp
#define plugin_proxy_hpp

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <string>

#include "plugin.hpp"
#include "message_queue_base.hpp"
#include "ne.hpp"

using boost::asio::ip::tcp;

class PluginProxy : public Plugin {
public:
    PluginProxy () {}
    virtual ~PluginProxy () {}

    virtual int initPlugin   (NE* environment_, const char* pluginName_);
    virtual int deinitPlugin (NE* environment_);

    virtual int initInstance (int id_);
    virtual int deinitInstance (int id_);

    virtual void* getMessage ();
    virtual int   execMessage (void* message_);

private:
    void _routine ();

private:
    typedef MessageQueueBase<void> MessageQueue;

    NE*                     _environment;   ///< Noodle Engine environment
    int                     _timeout;       ///< the timeout to wait for message
    MessageQueue            _messageQueue;  ///< message queue
    boost::asio::io_service _io_service;    ///< boost::asio::io_service
    boost::thread*          _thread;        ///< boost::thread
    std::string             _pluginName;    ///< plugin name
    std::string             _ip;            ///< the IP of STUB
    int                     _port;          ///< the port of STUB
    bool                    _quit;          ///< thread QUIT flag
};

#endif // plugin_proxy_hpp
