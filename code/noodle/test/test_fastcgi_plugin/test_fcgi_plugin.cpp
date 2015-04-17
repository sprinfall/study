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

#include <ostream> 

#include "noodle.hpp"
#include "test_fcgi_plugin.hpp"
#include "fastcgi_message.hpp"

DECLARE_NOODLE_PLUGIN (TestFcgiPlugin);

int TestFcgiPlugin::initPlugin (NE *environment_, const char* pluginName_) {
    _environment = environment_;
    return OK;
}

int TestFcgiPlugin::deinitPlugin (NE *environment_) {
    return OK;
}

int TestFcgiPlugin::deinitInstance (int id_) {
    return OK;
}

int TestFcgiPlugin::initInstance (int id_) {
    return OK;
}

void* TestFcgiPlugin::getMessage () {
    return 0;
}

int TestFcgiPlugin::execMessage (void *message_) {

    _environment->setStation (message_,
        _environment->getInstanceIdentifier ("fastcgi"));

    FastCGIMessage* fcgiMessage =
        reinterpret_cast<FastCGIMessage*>
        (
            _environment->getAttachment (message_)
        );

    ostreambuf<char> ostreamBuffer (fcgiMessage->out.response,
        fcgiMessage->out.length); 
    std::ostream messageStream (&ostreamBuffer); 

    messageStream
            << "Set-Cookie: ne=Noodle Engine Test Cookie\n"
            << "Content-type: text/html\r\n\r\n"
            << "<html><head><meta http-equiv='Content-Type' content='text/html; charset=utf-8' />"
            << "<title>FastCGI echo from Noodle Engine</title>"
            << "<h1>Session Parameters</h1>"
            << "<p><b>Server Name:</b>"  << fcgiMessage->in.serverName  << "<br />"
            << "<b>Host:          </b>"  << fcgiMessage->in.host        << "<br />"
            << "<b>URI:           </b>"  << fcgiMessage->in.uri         << "<br />"
            << "<b>Query String:  </b>"  << fcgiMessage->in.queryString << "<br />"
            << "<b>Referer:       </b>"  << fcgiMessage->in.referer     << "<br />"
            << "<b>Remote IP:     </b>"  << fcgiMessage->in.remoteIP    << "<br />"
            << "<b>Remote Port:   </b>"  << fcgiMessage->in.remotePort  << "<br />"
            << "<b>User Agent:    </b>"  << fcgiMessage->in.userAgent   << "<br />"
            << "<b>Cookie:        </b>"  << fcgiMessage->in.cookie      << "</p>" << '\0';

    fcgiMessage->out.length = strlen (fcgiMessage->out.response) + 1;

    return OK;
}
