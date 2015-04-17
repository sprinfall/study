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
 \file proxy_manager.hpp
 \brief proxy manager
 */

#ifndef proxy_manager_hpp
#define proxy_manager_hpp

#include <map>
#include <list>
#include <boost/thread/mutex.hpp>
#include "proxy.hpp"

class ModuleManager;

/*!
 \brief proxy manager
 */

class ProxyManager {
public:

    /*!
     \brief constructor
     \param moduleManager_ module manager
     */

    ProxyManager (ModuleManager* moduleManager_)
        : _moduleManager (moduleManager_) {}

    /*!
     \brief destructor
     */

    ~ProxyManager ();

    /*!
     \brief create proxy
     \param instanceID_ instance ID
     \return proxy object
     */

    Proxy* createProxy (int instanceID_);

    /*!
     \brief destroy proxy
     \param proxy_ Proxy pointer
     \return OK or error code
     */

    int destroyProxy (Proxy* proxy_);

    /*!
     \brief destroy proxy by plugin name
     \param pluginName_ plugin name
     \return OK or error code
     */

    int destroyProxy (const std::string& pluginName_);

    /*!
     \brief destroy source proxy by plugin name
     \param pluginName_ plugin name
     \return OK or error code
     */

    int destroySourceProxy (const std::string& pluginName_);
    
private:
    typedef std::list <Proxy*>        ProxyList;
    typedef std::map <int, ProxyList> ProxyTable;

    boost::mutex   _mutex;         ///< binary lock
    ModuleManager* _moduleManager; ///< module manager
    ProxyTable     _proxyTable;    ///< proxy table
};

#endif // proxy_manager_hpp
