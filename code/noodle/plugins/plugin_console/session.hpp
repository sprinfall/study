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

#ifdef WIN32
#   define _WIN32_WINNT 0x0501
#   pragma warning(disable:4996)
#endif

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <string>
#include <vector>

#include "ne.hpp"
#include "server.hpp"

class PluginConsole;
using namespace boost::asio::ip;

class Session
{
public:
    Session (Server* server_, NE* environment_,
        PluginConsole* console_,
        boost::asio::io_service& io_service_);
    ~Session () {}

    void start ();
    void read (const boost::system::error_code& error, size_t bytes);
    void write (const boost::system::error_code& error);

    inline tcp::socket& getSocket() {
        return _socket;
    }

private:
    typedef std::vector<std::string> split_vector_type;

    bool _verifyCompletion (char* buf, size_t length_);
    void _writeMessage (const char* message_);
    void _postSimpleCoreMessage (Command* command_);
    bool _execute (char* cmd_);
    bool _onHelp (split_vector_type& splitCmd_);
    bool _onStop (split_vector_type& splitCmd_);
    bool _onStart (split_vector_type& splitCmd_);
    bool _onEnable (split_vector_type& splitCmd_);
    bool _onDisable (split_vector_type& splitCmd_);
    bool _onInstall (split_vector_type& splitCmd_);
    bool _onUninstall (split_vector_type& splitCmd_);
    bool _onUpdate (split_vector_type& splitCmd_);
    bool _onShutdown (split_vector_type& splitCmd_);
    bool _onStat (split_vector_type& splitCmd_);

private:
    static const size_t MAX_READ_BUFFER_LENGTH  = 512;
    static const size_t MAX_WRITE_BUFFER_LENGTH = 102400;

    Server*        _server;
    NE*            _environment;
    PluginConsole* _console;
    tcp::socket    _socket;
    size_t         _rbufLength;
    char           _rbuf[MAX_READ_BUFFER_LENGTH];
    char           _wbuf[MAX_WRITE_BUFFER_LENGTH];
};
