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
 \file module_manager.hpp
 \brief module manager
 */

#ifndef module_manager_hpp
#define module_manager_hpp

#include <string>
#include <map>
#include <boost/thread/mutex.hpp>
#include <sstream>
#include "module.hpp"
#include "proxy_manager.hpp"

/*!
 \brief manage all loaded module
 */
class ModuleManager {
public:
    /*!
     \brief constructor
     */
    ModuleManager () {}

    /*!
     \brief destructor
     */
    ~ModuleManager ();

    /*!
     \brief load a module according to specific name_ and path_
     \param name_ module name
     \param path_ module path(not include module name)
     \param source_ whether install a source plugin
     \retval ERR_LOAD_MODULE cannot load module
     \retval ERR_DUP_MODULE duplicate module
     \retval OK success
     */
    int installModule (const std::string& name_, const std::string& path_, bool source_ = false);

    /*!
     \brief unload a module according to specific name_ and path_
     \param name_ module name_
     \retval ERR_UNKNOWN_MODULE unknown module
     \retval OK success
     */
    int uninstallModule (const std::string& name_);

    /*!
     \brief update specific module

            when updating the module it will STATE_SUSPEND all invoking to the instances which forks by this module
     \param name_ the module need to update
     \param updatedName_ the old module will be updated by this module
     \param path_ module path(not include module name)
     \retval ERR_UNKNOWN_MODULE unknown module
     \retval OK success
     */
    int updateModule (const std::string& name_, const std::string& updatedName_,
        const std::string& path_);

    /*!
     \brief create a instance which forked from name_ by instance ID id_
     \param name_ plugin name
     \param id_ instance ID
     \retval OK success
     */

    int createInstance (const std::string& name_, int id_);

    /*!
     \brief destroy instance
     \param id_ instance ID
     \retval OK success
     */

    int destroyInstance (int id_);

    /*!
     \brief disable instance
     \param id_ instance ID
     \retval OK success
     */

    int disableInstance (int id_);

    /*!
     \brief enable instance
     \param id_ instance ID
     \retval OK success
     */

    int enableInstance (int id_);

    /*!
     \brief enable instance
     \param name_ plugin name
     \return true or false
     */

    bool sourcable (const std::string& name_);

    /*!
     \brief randomly get a instance ID from plugin
     \param name_ plugin name
     \retval instance ID
     */

    int getRandomInstanceID (const std::string& name_);

    /*!
     \brief print tree of plugin's children to std::stringstream
     \param ostream_ std::stringstream
     */

    void printTree (std::stringstream& ostream_);

    friend class ProxyManager;

private:
    Proxy* createProxy (int id_);
    int destroyProxy (Proxy* proxy_, int id_);

private:
    typedef std::map <std::string, Module*> ModuleTable;
    typedef std::map <int, Module*>         i2mTable;
    typedef std::map <int, Instance*>       InstanceTable;

    boost::mutex  _mutex;         ///< binary lock
    ModuleTable   _moduleTable;   ///< module table
    i2mTable      _i2mTable;      ///< instance to module table
    InstanceTable _instanceTable; ///< instance table
};

#endif // module_manager_hpp
