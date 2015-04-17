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
 \file instance.hpp
 \brief plugin's instance
 */

#ifndef instance_hpp
#define instance_hpp

#include <cassert>
#include <string>

#include "subject.hpp"
#include "plugin.hpp"

/*!
 \brief plugin's instance
 */

class Instance : public Subject {
public:

    /*!
     \brief constructor
     \param plugin_ plugin pointer
     \param instanceID_ instance identifier
     \param pluginName_ plugin name
     \param source_ whether is source plugin
     */

    Instance (Plugin* plugin_, int instanceID_,
        const std::string& pluginName_, bool source_ = false)
        : _plugin (plugin_),
          _instanceID (instanceID_),
          _pluginName (pluginName_),
          _source (source_) { assert (plugin_); }

    /*!
     \brief destructor
     */

    virtual ~Instance () {}

    /*!
     \brief put a message to instance
     \param message_ message
     \return OK or error code, get the error detail by ErrorString::errorString
     */

    virtual int putMessage (Message* message_);

    /*!
     \brief get a message from instance
     \return message
     */

    virtual Message* getMessage ();

    /*!
     \brief reset plugin
     \param plugin_ plugin pointer
     \return OK or error code, get the error detail by ErrorString::errorString
     */

    int reset (Plugin* plugin_);

    /*!
     \brief get instance identifier
     \return instance identifier
     */

    inline int getInstanceID () {
        return _instanceID;
    }

    /*!
     \brief get plugin name
     \return plugin name
     */

    inline const std::string& getPluginName () {
        return _pluginName;
    }

    /*!
     \brief whether is source
     \return true or false
     */

    inline bool getSource () {
        return _source;
    }

private:
    std::string _pluginName; ///< plugin name
    Plugin*     _plugin;     ///< plugin pointer
    int         _instanceID; ///< instance identifier
    bool        _source;     ///< is source?
};

#endif // instance_hpp
