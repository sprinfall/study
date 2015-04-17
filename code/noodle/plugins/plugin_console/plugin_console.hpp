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
 \file plugin_console.hpp
 \brief plugin console
 */

#ifndef plugin_console_hpp
#define plugin_console_hpp

#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <string>

#include "plugin_source.hpp"
#include "server.hpp"

/*!
 \brief plugin console
 */

class PluginConsole : public PluginSource {
public:

    /*!
     \brief constructor
     */

    PluginConsole ()
        : _server (0), _quit (false), _exec(false), _thread (0) {}

    /*!
     \brief destructor
     */

    virtual ~PluginConsole () {}

    virtual int initPlugin   (NE* environment_, const char* pluginName_);
    virtual int deinitPlugin (NE* environment_);

    virtual int getMessageInternal ();
    virtual int execMessageInternal (void *message_);

    /*!
     \brief wait for result from noodle engine
     */

    void wait ();

    /*!
     \brief get rusult string
     \return result string
     */

    std::string getResult ();

    /*!
     \brief plugin thread routine
     */

    void routine ();

private:
    bool              _quit;
    bool              _exec;
    Server*           _server;
    std::string       _result;
    boost::thread*    _thread;
    boost::mutex      _quitMutex;
    boost::condition  _quitCondition;
    boost::mutex      _execMutex;
    boost::condition  _execCondition;
};

#endif // plugin_console_hpp
