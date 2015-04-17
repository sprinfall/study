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

#ifndef plugin_source_hpp
#define plugin_source_hpp

#include "plugin.hpp"
#include "message_queue_base.hpp"
#include "ne.hpp"

class PluginSource : public Plugin {
public:
    PluginSource () {}
    virtual ~PluginSource () {}

    virtual int initPlugin   (NE* environment_, const char* pluginName_);
    virtual int deinitPlugin (NE* environment_);

    virtual int initInstance (int id_);
    virtual int deinitInstance (int id_);

    virtual void* getMessage ();
    virtual int   execMessage (void* message_);

    /*!
     \brief put message into message queue of source
     \param message_ Message pointer
     \param urgent_ insert message to the front of message queue
     \return OK or error code
     */

    int putMessage (void* message_, bool urgent_ = false);

    /*!
     \brief get noodle environment     
     \return environment pointer
     */

    inline NE* getEnvironment () {
        return _environment;
    }

private:

    /*!
     \brief get message from child class, child class should put message by
            call PluginSource::putMessage
     \return OK or error code
     */

    virtual int getMessageInternal () = 0;

    /*!
     \brief child class execute a message from noodle engine
     \param message_ Message pointer
     \return OK or error code
     */

    virtual int execMessageInternal (void *message_) = 0;

protected:
    typedef MessageQueueBase<void> MessageQueue;

    MessageQueue _messageQueue; ///< message queue
    NE*          _environment;  ///< noodle environment
};

#endif // plugin_source_hpp
