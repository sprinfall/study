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

#include <stdexcept>
#include <cassert>

#include "server.hpp"
#include "session.hpp"

Server::Server (NE* environment_, PluginConsole* console_,
    const char *ip_, short port_)
    : _environment (environment_),
      _console (console_),
      _ip (ip_),
      _port (port_) {
    assert (environment_);
    assert (port_);
    boost::system::error_code error;

#ifdef IPV6
    address_v6 addr = address_v6::from_string (ip_, error);
#else
    address_v4 addr = address_v4::from_string (ip_, error);
#endif
    if (error) {
        throw std::runtime_error ("invalid ip address");
    }

    basic_endpoint<tcp> point (addr, _port);
    _acceptor = new tcp::acceptor (_io_service, point);
}

Server::~Server () {
    delete _acceptor;
}

void Server::start () {
    Session* session = new Session (this, _environment, _console,
        _io_service);
    _acceptor->async_accept (
        session->getSocket (),
        boost::bind (&Server::_accept, this, session,
            boost::asio::placeholders::error));
    _io_service.run ();
}

void Server::stop() {
    _io_service.stop ();
}

void Server::_accept (Session* session_,
    const boost::system::error_code& error_) {
    if (!error_) {
        session_->start();
        session_ = new Session (this, _environment, _console, _io_service);
        _acceptor->async_accept (
            session_->getSocket (),
            boost::bind (&Server::_accept, this, session_,
                boost::asio::placeholders::error));
    } else {
        delete session_;
    }
}
