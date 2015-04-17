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
 \file module.hpp
 \brief module
 */

#ifndef module_hpp
#define module_hpp

#include <string>
#include <vector>
#include <boost/thread/mutex.hpp>
#include <sstream>
#include "ne.hpp"
#include "instance.hpp"
#include "os_misc.hpp"

#define HOOKPROC "hookProc"
typedef Plugin* (*ModuleProc)();

/*!
 \brief Module
 */

class Module {
public:

    /*!
     \brief constructor
     \param name_ plugin name
     \param path_ plugin full path
     \param source_ Is a source?
     */

    Module (const std::string& name_, const std::string& path_, bool source_ = false);

    /*!
     \brief destructor
     */

    virtual ~Module ();

    /*!
     \brief update plugin
     \param name_ plugin name
     \param path_ plugin full path
     \return ok or error code
     */

    int update (const std::string& name_, const std::string& path_);

    /*!
     \brief create a instance
     \param instanceID_ instance ID
     \return Instance pointer
     */

    Instance* createInstance (int instanceID_);

    /*!
     \brief destroy a instance
     \param instance_ Instance pointer
     \return ok or error code
     */

    int destroyInstance (Instance* instance_);

    /*!
     \brief randomly get a instance ID
     \return instance ID
     */

    int getRandomInstanceID ();

    /*!
     \brief get source flag
     \return true or false
     */

    inline bool sourcable () {
        return _source;
    }

    /*!
     \brief get plugin name
     \return plugin name
     */

    inline const std::string& getModuleName () {
        return _name;
    }

    /*!
     \brief get full name of plugin
     \return full name of plugin
     */

    inline const std::string& getFullName () {
        return _fullName;
    }

    /*!
     \brief print instance tree to std::stringstream
     */

    void printTree (std::stringstream& ostream_);

private:
    typedef std::vector<Instance*> InstanceList;

    Handle       _so;           ///< plugin .so or .dll
    Plugin*      _plugin;       ///< plugin object
    InstanceList _instanceList; ///< instance list
    bool         _source;       ///< source flag
    std::string  _name;         ///< plugin name
    std::string  _path;         ///< plugin path
    std::string  _fullName;     ///< plugin full name
};

#endif // module_hpp
