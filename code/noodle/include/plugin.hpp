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
 \file plugin.hpp
 \brief the base class of plugin
 */

#ifndef plugin_hpp
#define plugin_hpp

#include "ne.hpp"

/*!
 \brief the base class of plugin
 */

class Plugin {
public:

    /*!
     \brief constructor
     */

    Plugin () {}

    /*!
     \brief destructor
     */

    virtual ~Plugin () {}

    /*!
     \brief initialize plugin
     \param environment_ noodle environment pointer
     \param pluginName_ plugin name
     \return OK or error code
     */

    virtual int initPlugin   (NE* environment_, const char* pluginName_) = 0;

    /*!
     \brief deinitialize plugin
     \param environment_ noodle environment pointer
     \return OK or error code
     */

    virtual int deinitPlugin (NE* environment_) = 0;

    /*!
     \brief initialize instance
     \param id_ instance identifier
     \return OK or error code
     */

    virtual int initInstance (int id_) = 0;

    /*!
     \brief deinitialize instance
     \param id_ instance identifier
     \return OK or error code
     */

    virtual int deinitInstance (int id_) = 0;

    //virtual int disableInstance (int id_) = 0; // system want to disable the instance, notify the plugin
    //virtual int enableInstance (int id_) = 0;  // system want to enable the instance, notify the plugin
    //virtual unsigned getVersion () = 0;        // this method should support the dependency validation of system
    //virtual int refreshConfig () = 0;          // system has refresh the config, notify the plugin

    /*!
     \brief get message from plugin
     \return message pointer
     */

    virtual void* getMessage () = 0;

    /*!
     \brief execute message
     \return OK or error code
     */

    virtual int execMessage (void* message_) = 0;
};

#endif
