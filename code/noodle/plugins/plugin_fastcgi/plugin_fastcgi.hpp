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

/*!
 \file plugin_fastcgi.hpp
 \brief the plugin to connect web server which supports FastCGI protocol
 */

#ifndef plugin_fastcgi_hpp
#define plugin_fastcgi_hpp

#include <map>

#include <list>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>

#include "ne.hpp"
#include "plugin_source.hpp"
#include "fastcgi_message.hpp"
#include "fcgiapp.h"

/*!
 \brief FastCGI plugin
 */

class PluginFastCGI : public PluginSource {
public:

    /*!
     \brief constructor
     */

    PluginFastCGI () {}

    /*!
     \brief destructor
     */

    virtual ~PluginFastCGI () {}

    virtual int initPlugin   (NE* environment_, const char* pluginName_);
    virtual int deinitPlugin (NE* environment_);

    virtual int getMessageInternal ();
    virtual int execMessageInternal (void *message_);

private:
    struct Worker {
        bool             flag;         ///< thread terminated flag
        int              minReqID;     ///< minimum request ID
        int              maxReqID;     ///< reserved
        boost::thread*   thread;       ///< boost::thread
        boost::mutex     mutex;        ///< lock for flag
        boost::condition condition;    ///< terminate trigger
        MessageQueue     messageQueue; ///< message queue
    };

    /*!
     \brief worker thread routine
     \param worker_ worker attrubutes
     */

    void routine (Worker* worker_);

    /*!
     \brief get HTTP header values
     \param fcgiMessage_ FastCGI message
     \param request_ FastCGI request
     */

    void getHttpValues (FastCGIMessage& fcgiMessage_, FCGX_Request& request_);

    bool acceptRequest (FCGX_Request** fcgiRequest_);

private:
    typedef std::list<Worker*>           ThreadList;
    typedef std::map<int, MessageQueue*> MessageQueueTable;

    ThreadList        _threadList;         ///< thread list
    MessageQueueTable _messageQueueTable;  ///< message queue fast find table
    std::string       _ip;                 ///< ip
    boost::mutex      _acceptMutex;        ///< protect FCGX_Accept_r
    boost::mutex      _tableMutex;         ///< protect _messageQueueTable
    bool              _shutdown;           ///< shutdown flag
    bool              _over;               ///< FastCGI connection terminated flag
    std::string       _reveiver;           ///< FastCGI message reveiver
    int               _listen;             ///< listen socket
    int               _port;               ///< listen port
    int               _concurrent;         ///< worker number
    int               _backLog;            ///< back log of listen
    int               _maxBuf;             ///< maximum buffer size to carry FastCGI message to reveiver
};

#endif // plugin_fastcgi_hpp
