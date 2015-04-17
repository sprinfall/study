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

#ifndef fastcgi_message_hpp
#define fastcgi_message_hpp

#include "fcgiapp.h"

enum {
    MAX_SERVER_NAME_LENGTH = 1024,
    MAX_HOST_LENGTH        = 256,
    MAX_USER_AGENT_LENGTH  = 256,
    MAX_REFERER_LENGTH     = 256,
    MAX_URI_LENGTH         = 256,
    MAX_REMOTE_IP_LENGTH   = 32,
    MAX_REMOTE_PORT        = 16,
    MAX_QUERY_STRING       = 256,
    MAX_COOKIE_LENGTH      = 4096
};

struct FastCGIMessage {
    int requestID;

    struct {
            char serverName[MAX_SERVER_NAME_LENGTH];
            char host[MAX_HOST_LENGTH];
            char userAgent[MAX_USER_AGENT_LENGTH];
            char referer[MAX_REFERER_LENGTH];
            char uri[MAX_URI_LENGTH];
            char remoteIP[MAX_REMOTE_IP_LENGTH];
            char remotePort[MAX_REMOTE_PORT];
            char queryString[MAX_QUERY_STRING];
            char cookie[MAX_COOKIE_LENGTH];
    } in;

    struct {
        int length;
        char response[1];
    } out;
};

#endif // fastcgi_message_hpp
