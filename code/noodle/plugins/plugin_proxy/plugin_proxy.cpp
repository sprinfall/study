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
#include <stdexcept>

#include "noodle.hpp"
#include "ne.hpp"
#include "plugin_proxy.hpp"
#include "proxy_stub_message.hpp"

DECLARE_NOODLE_PLUGIN (PluginProxy);

int PluginProxy::initPlugin (NE *environment_, const char* pluginName_) {
    assert (pluginName_);

    _environment = environment_;
    _pluginName  = pluginName_;
    _quit        = false;

    std::stringstream queryIP;
    std::stringstream queryPort;
    queryIP   << "proxy." << pluginName_ << ".ip";
    queryPort << "proxy." << pluginName_ << ".port";

    // TODO: validate config

    try {
        _ip      = _environment->getString  (queryIP.str ().c_str ());
        _port    = _environment->getInteger (queryPort.str ().c_str ());
        _timeout = _environment->getInteger ("proxy.timeout");
    } catch (std::exception& e) {
        _environment->writeErrorLog (e.what ());
        return FAIL;
    }

    _thread = new boost::thread (boost::bind (&PluginProxy::_routine, this));

    return OK;
}

int PluginProxy::deinitPlugin (NE *environment_) {
    _quit = true;
    _thread->join ();
    delete _thread;

    // cleanup all dirty messages

    void* message = 0;
    while (message = _messageQueue.tryGetMessage ()) {
        _environment->destroyMessage (message);
    }

    return OK;
}

int PluginProxy::deinitInstance (int id_) {
    return OK;
}

int PluginProxy::initInstance (int id_) {
    return OK;
}

void* PluginProxy::getMessage () {
    return 0;
}

int PluginProxy::execMessage (void *message_) {
    void* message =
        _environment->createMessage (
            0, 0,
            _environment->getAttachment (message_),
            _environment->getAttachmentLength (message_));

    _messageQueue.putMessage (message);

    return DONE;
}

void PluginProxy::_routine () {
    tcp::resolver resolver(_io_service);

    std::stringstream serverName;
    serverName << _port;

START:

    {
        // TODO: add a config for reconnect interval
        // make a TCP connection to STUB

        tcp::socket s (_io_service);
        try {
#ifdef IPV6
            tcp::resolver::query query(tcp::v6(), _ip, serverName.str ());
#else
            tcp::resolver::query query(tcp::v4(), _ip, serverName.str ());
#endif
            tcp::resolver::iterator iterator = resolver.resolve (query);
            s.connect (*iterator);
        } catch (std::exception& e) {
            // TODO: reconnect stub
            if (_quit) {
                _environment->writeNoticeLog ("Proxy routine terminated");
                return;
            }
            _environment->writeErrorLog (e.what ());
#ifdef WIN32
            ::Sleep (1000);
#else
            ::usleep (1000000);
#endif
            goto START;
        }

        // main loop

        for (;!_quit;) {

            // get message from Noodle Engine

            void* message = _messageQueue.getMessage (_timeout);

            if (!message) {

                // no message arrived

                continue;
            }

            // allocates a message and fill it

            void* attachment     = _environment->getAttachment (message);
            int attachmentLength = _environment->getAttachmentLength (message);
            int totalLength      =
                attachmentLength + sizeof (ProxyStubMessageHeader);

            ProxyStubMessage* proxyStubMessage =
                static_cast<ProxyStubMessage*>(
                    _environment->alloc (totalLength));

            memcpy (proxyStubMessage->header.moduleName, _pluginName.c_str (),
                _pluginName.size () + 1);

            proxyStubMessage->header.length = attachmentLength;

            if (attachment) {
                memcpy (proxyStubMessage->body.data, attachment,
                    attachmentLength);
            }

            // send entire message to STUB

            boost::system::error_code error;

            try {
                boost::asio::write(
                    s,
                    boost::asio::buffer((char*)proxyStubMessage, totalLength),
                    boost::asio::transfer_all(),
                    error);
            } catch (std::exception& e) {

                // meet some critical error, try to restart loop

                _environment->writeLog (ERR, e.what ());
                _environment->free (proxyStubMessage, totalLength);
                goto START;
            }

            // free message

            _environment->free (proxyStubMessage, totalLength);
            _environment->destroyMessage (message);

            if (error) {

                // meet some socket error, try to restart loop

                _environment->writeErrorLog (
                    boost::system::system_error (error).what ());
                goto START;
            }
        }
    }

    _environment->writeNoticeLog ("Proxy routine terminated");
}
