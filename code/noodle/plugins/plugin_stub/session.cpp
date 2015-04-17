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

#include "session.hpp"
#include "plugin_stub.hpp"
#include "proxy_stub_message.hpp"

Session::Session (Server* server_, NE* environment_,
    PluginStub* stub_,
    boost::asio::io_service& io_service_)
    : _server (server_),
      _environment (environment_),
      _stub (stub_),
      _socket (io_service_),
      _headerLength (0),
      _header (0),
      _bodyLength (0),
      _realBodyLength (0),
      _body (0) {

   // allocates memory for header

    _header =
        static_cast<char*>(
            _environment->alloc (sizeof (ProxyStubMessageHeader)));
}

Session::~Session () {
    _environment->free (_header, sizeof (ProxyStubMessageHeader));
    if (_bodyLength) {
        _environment->free (_body, _bodyLength);
    }
}

void Session::start() {    
    boost::asio::async_read (
        _socket,
        boost::asio::buffer(_header, sizeof (ProxyStubMessageHeader)),
        boost::bind(
            &Session::readHeader,
            this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
}

void Session::readHeader (const boost::system::error_code& error,
    size_t bytes) {
    if (!error) {
        ProxyStubMessageHeader* header =
            reinterpret_cast<ProxyStubMessageHeader*>(_header);

        if (_bodyLength < (size_t)(header->length)) {
            if (_bodyLength) {
                _environment->free (_body, _bodyLength);
            }
            _bodyLength = header->length;
            _realBodyLength = header->length;
            _body = static_cast<char*>(_environment->alloc (_bodyLength));
        } else {
            _realBodyLength = header->length;
        }

        // read body

        boost::asio::async_read(
                _socket,
                boost::asio::buffer(
                    _body,
                    _realBodyLength),
                boost::bind(
                    &Session::readBody,
                    this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
    } else {
        _environment->writeErrorLog (
            "%s, read message header failed",
            error.message ().c_str ());
        delete this;
    }
}

void Session::readBody (const boost::system::error_code& error,
    size_t bytes) {
    if (!error) {

        ProxyStubMessageHeader* header =
            reinterpret_cast<ProxyStubMessageHeader*>(_header);

        int receiverID =
            _environment->getInstanceIdentifier (header->moduleName);

        //TODO: check plugin name

        if (!receiverID) {
            _environment->writeErrorLog (
                "Inexist plugin: %s",
                header->moduleName);

            // read next header

            memset (_header, 0, sizeof (ProxyStubMessageHeader));
            boost::asio::async_read (
                _socket,
                boost::asio::buffer(_header, sizeof (ProxyStubMessageHeader)),
                boost::bind(
                    &Session::readHeader,
                    this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
            return;
        }

        void* message =
            _environment->createMessage (
                MSG_CALL,
                receiverID,
                _body,
                _bodyLength);

        // send message to Noodle Engine

        _stub->putMessage (message);

        // read next header
        memset (_header, 0, sizeof (ProxyStubMessageHeader));
        boost::asio::async_read (
            _socket,
            boost::asio::buffer(_header, sizeof (ProxyStubMessageHeader)),
            boost::bind(
                &Session::readHeader,
                this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
    } else {
        _environment->writeErrorLog (
            "%s, read message body failed",
            error.message ().c_str ());
        delete this;
    }
}
