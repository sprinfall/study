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
#include <boost/bind.hpp>

#include "plugin_fastcgi.hpp"

#ifdef WIN32
#pragma comment(lib,"ws2_32.lib")
#include <windows.h>
#endif

DECLARE_NOODLE_PLUGIN (PluginFastCGI);

int PluginFastCGI::initPlugin (NE* environment_, const char* pluginName_) {
    PluginSource::initPlugin (environment_, pluginName_);

    // NOTICE: for now this plugin cannot be reinstall
    // TODO: add some validation for this configs

    try {
        _ip         = environment_->getString  ("fastcgi.ip");
        _port       = environment_->getInteger ("fastcgi.port");
        _concurrent = environment_->getInteger ("fastcgi.concurrent");
        _backLog    = environment_->getInteger ("fastcgi.backLog");
        _reveiver   = environment_->getString  ("fastcgi.receiver");
        _maxBuf     = environment_->getInteger ("fastcgi.maxBufferLength");
    } catch (std::exception& e) {
        environment_->writeLog (
            ERR, e.what ());
        return FAIL;
    }

    _concurrent = _concurrent > 0 ? _concurrent : 1;
    _backLog    = _backLog > 0 ? _backLog : 100;
    _maxBuf     = _maxBuf >= sizeof (FastCGIMessage) ?
                      _maxBuf : sizeof (FastCGIMessage);

    if (_reveiver.empty ()) {
        environment_->writeErrorLog (
            "need to set fastcgi.receiver");
        return FAIL;
    }

    // TODO: to open a custom socket
    //       fastcgi FCGX_OpenSocket only support [localhost]:port
    //       or unix socket path

#ifdef WIN32
    if (("localhost" != _ip) && !_ip.empty ()) {
        environment_->writeErrorLog (
            "invalid ip, use localhost or empty");
        return FAIL;
    }
#endif

    std::stringstream sstream;
    sstream << _ip << ":" << _port;    

    if (FCGX_Init ()) {
        environment_->writeErrorLog (
            "fail to initialize fastCGI");
        return FAIL;
    }

    _shutdown = false;
    _over     = false;

    _listen = FCGX_OpenSocket (sstream.str ().c_str (), _backLog);

    if (_listen < 0) {
        environment_->writeErrorLog (
            "fail to create listen socket for fastCGI");
        return FAIL;
    }

    int reqID = 1;
    for (int i = 0; i < _concurrent; i++, reqID++) {
        Worker* worker = new Worker;

        worker->flag = false;
        worker->minReqID = reqID;
        worker->maxReqID = reqID + 1; // reserved
        worker->thread =
            new boost::thread
            (
                boost::bind
                (
                    &PluginFastCGI::routine,
                    this,
                    worker
                )
            );
         
        _threadList.push_back (worker);
    }

    return OK;
}

int PluginFastCGI::deinitPlugin (NE* environment_) {
    // shutdown fastcgi library
    FCGX_ShutdownPending ();
    _shutdown = true;

    ThreadList::iterator tlIt = _threadList.begin ();
    for (; tlIt != _threadList.end (); tlIt++) {
        do {
            boost::mutex::scoped_lock lock ((*tlIt)->mutex);
            while (!(*tlIt)->flag) {
                (*tlIt)->condition.wait (lock);
            }
        } while (0);

        (*tlIt)->thread->join ();

        delete (*tlIt)->thread;
        delete *tlIt;
    }

#ifdef WIN32
    ::closesocket (_listen);
#else
    close (_listen);
#endif

    PluginSource::deinitPlugin (environment_);

    return OK;
}

int PluginFastCGI::getMessageInternal () {
    return OK;
}

int PluginFastCGI::execMessageInternal (void *message_) {

    NE* ne = PluginSource::getEnvironment();

    // get a message from noodle engine

    FastCGIMessage* fcgiMessage =
        reinterpret_cast<FastCGIMessage*>
        (
            PluginSource::getEnvironment()->getAttachment (message_)
        );

    boost::mutex::scoped_lock lock (_tableMutex);

    MessageQueueTable::iterator mqtIt =
        _messageQueueTable.find (fcgiMessage->requestID);

    if (mqtIt == _messageQueueTable.end ()) {
        ne->writeErrorLog (
            "invalid request identifier: %d", fcgiMessage->requestID);
    } else {

        // clone a new message, because
        // engine will free the message
        // after return OK

        void* message =
            ne->createMessage
            (
                0, 0, 0, // we donot care about this
                ne->getAttachmentLength (message_)
            );

        FastCGIMessage* out = static_cast<FastCGIMessage*> (
            ne->getAttachment (message));

        memcpy (out->out.response, fcgiMessage->out.response,
            fcgiMessage->out.length);

        mqtIt->second->putMessage (message);

        // remove it from message queue table

        _messageQueueTable.erase (mqtIt);
    }

    return DONE;
}

void PluginFastCGI::getHttpValues (FastCGIMessage& fcgiMessage_, FCGX_Request& request_) {

    ///////////////////////////////
    // The values of HTTP header //
    ///////////////////////////////
    // HTTP_HOST                 //
    // HTTP_USER_AGENT           //
    // HTTP_ACCEPT               //
    // HTTP_ACCEPT_LANGUAGE      //
    // HTTP_ACCEPT_CHARSET       //
    // HTTP_REFERER              //
    // CONTENT_TYPE              //
    // HTTP_COOKIE               //
    // DOCUMENT_ROOT             //
    // SCRIPT_NAME               //
    // QUERY_STRING              //
    // HTTP_KEEP_ALIVE           //
    // CONTENT_LENGTH            //
    // SERVER_ADDR               //
    // REMOTE_ADDR               //
    // SERVER_PORT               //
    // REMOTE_PORT               //
    // HTTP_IF_MODIFIED_SINCE    //
    // HTTP_IF_NONE_MATCH        //
    ///////////////////////////////

    fcgiMessage_.out.length = _maxBuf - sizeof (FastCGIMessage);

    char* serverName = FCGX_GetParam("SERVER_NAME", request_.envp);
    if (serverName) {
        memcpy (fcgiMessage_.in.serverName, serverName,
            strlen (serverName) + 1);
    } else {
        fcgiMessage_.in.serverName[0] = '\0';
    }

    char* host = FCGX_GetParam("HTTP_HOST", request_.envp);
    if (host) {
        memcpy (fcgiMessage_.in.host, host, strlen (host) + 1);
    } else {
        fcgiMessage_.in.host[0] = '\0';
    }

    char* userAgent = FCGX_GetParam("HTTP_USER_AGENT",
        request_.envp);
    if (userAgent) {
        memcpy (fcgiMessage_.in.userAgent, userAgent,
            strlen (userAgent) + 1);
    } else {
        fcgiMessage_.in.userAgent[0] = '\0';
    }

    char* referer = FCGX_GetParam("HTTP_REFERER",
        request_.envp);
    if (referer) {
        memcpy (fcgiMessage_.in.referer, referer,
            strlen (referer) + 1);
    } else {
        fcgiMessage_.in.referer[0] = '\0';
    }

    char* uri = FCGX_GetParam("REQUEST_URI",
        request_.envp);
    if (uri) {
        memcpy (fcgiMessage_.in.uri, uri,
            strlen (uri) + 1);
    } else {
        fcgiMessage_.in.uri[0] = '\0';
    }

    char* queryString = FCGX_GetParam("QUERY_STRING",
        request_.envp);
    if (queryString) {
        memcpy (fcgiMessage_.in.queryString, queryString,
            strlen (queryString) + 1);
    } else {
        fcgiMessage_.in.queryString[0] = '\0';
    }

    char* remoteIP = FCGX_GetParam("REMOTE_ADDR",
        request_.envp);
    if (remoteIP) {
        memcpy (fcgiMessage_.in.remoteIP, remoteIP,
            strlen (remoteIP) + 1);
    } else {
        fcgiMessage_.in.remoteIP[0] = '\0';
    }

    char* remotePort = FCGX_GetParam("REMOTE_PORT",
        request_.envp);
    if (remotePort) {
        memcpy (fcgiMessage_.in.remotePort, remotePort,
            strlen (remotePort) + 1);
    } else {
        fcgiMessage_.in.remotePort[0] = '\0';
    }

    char* cookie = FCGX_GetParam("HTTP_COOKIE",
        request_.envp);
    if (cookie) {
        memcpy (fcgiMessage_.in.cookie, cookie,
            strlen (cookie) + 1);
    } else {
        fcgiMessage_.in.cookie[0] = '\0';
    }
}

void PluginFastCGI::routine (Worker* worker_) {
    int             rc       = 0;
    NE*             ne       = PluginSource::getEnvironment();
    int             curReqID = worker_->minReqID;
    FCGX_Request    request;

#ifdef WIN32
    FCGX_InitRequest(&request, _listen, 0);
#else

    // use signal to break accept loop

    FCGX_InitRequest(&request, _listen, FCGI_FAIL_ACCEPT_ON_INTR);
#endif

    for (;;) {
        {
            boost::mutex::scoped_lock lock (_acceptMutex);

            // FCGX_Accept_r will be called by multiple thread
            // use flag _over to avoid calling OS_LibShutdown
            // more than once, otherwise, system will crash

            if (!_over) {
                rc = FCGX_Accept_r(&request);
            } else {

                // free last message

                //FCGX_Finish_r(&request);

                ne->writeNoticeLog (
                    "fastCGI worker terminated");

                goto QUIT;
            }

            if (rc < 0) {
                if (_shutdown) {
                    _over = true;

                    ne->writeNoticeLog (
                        "fastCGI worker terminated");

                    FCGX_Finish_r(&request);

                    goto QUIT;
                } else {
#ifdef WIN32
                    ne->writeErrorLog (
                        "unknown error from fastCGI");
#else                    
                    // listener socket accept timeout
#endif
                    continue;
                }
            }
        }

        // get fastCGI source plugin instance ID

        int sourceID = ne->getInstanceIdentifier ("fastcgi");

        // get receiver plugin instance ID

        int receiverID = ne->getInstanceIdentifier (_reveiver.c_str ());

        // create a message for receiver

        // actually, need a large buffer than fcgiMessage
        // it's configurable by fastcgi.maxBufferLength

        void* message =
            ne->createMessage
            (
                MSG_CALL,
                receiverID,
                0,
                _maxBuf
            );

        FastCGIMessage* fcgiMessage = static_cast<FastCGIMessage*>(
            ne->getAttachment (message));

        // set request ID

        fcgiMessage->requestID = curReqID;

        // get HTTP variables

        getHttpValues (*fcgiMessage, request);

        // add to message queue table

        {
            boost::mutex::scoped_lock lock (_tableMutex);
            _messageQueueTable.insert (
                std::make_pair (curReqID, &worker_->messageQueue));
        }

        putMessage (message);

        // waiting for noodle engine's response

        void* response = 0;

        FastCGIMessage* fcgiResponse =
            reinterpret_cast<FastCGIMessage*>
            (
                ne->getAttachment
                (
                    response = worker_->messageQueue.getMessage ()
                )
            );

        if (fcgiResponse) {
            FCGX_FPrintF (request.out, fcgiResponse->out.response);

            // free message

            ne->destroyMessage (response);
        } else {
            ne->writeErrorLog (
                "get a null HTTP response");
        }

        FCGX_Finish_r(&request);
    }

QUIT:

    boost::mutex::scoped_lock lock (worker_->mutex);
    worker_->flag = true;
    worker_->condition.notify_all ();
}
