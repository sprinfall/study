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

#ifndef session_hpp
#define session_hpp

#include <boost/bind.hpp>
#include <boost/asio.hpp>

#include "ne.hpp"
#include "server.hpp"

class PluginStub;

class Session
{
public:
    Session (Server* server_, NE* environment_,
        PluginStub* stub_,
        boost::asio::io_service& io_service_);
    ~Session ();

    void start ();

    inline tcp::socket& getSocket() {
        return _socket;
    }

private:
    void readHeader (const boost::system::error_code& error, size_t bytes);
    void readBody (const boost::system::error_code& error, size_t bytes);

private:
    Server*        _server;
    NE*            _environment;
    PluginStub*    _stub;
    tcp::socket    _socket;
    size_t         _headerLength;
    char*          _header;
    size_t         _bodyLength;
    size_t         _realBodyLength;
    char*          _body;
};

#endif // session_hpp
