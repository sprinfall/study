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
 \file proxy_manager.cpp
 \brief implement proxy manager
 */

#include <cassert>
#include <algorithm>

#include "global_object_manager.hpp"
#include "proxy_manager.hpp"
#include "module_manager.hpp"
#include "message_source_manager.hpp"

ProxyManager::~ProxyManager () {
    boost::mutex::scoped_lock lock (_mutex);
    ProxyTable::iterator ptIt = _proxyTable.begin ();
    for (; ptIt != _proxyTable.end (); ptIt++) {
        ProxyList::iterator plIt = ptIt->second.begin ();
        for (; plIt != ptIt->second.end (); plIt++) {
            delete *plIt;
        }
        ptIt->second.clear ();
    }
    _proxyTable.clear ();
}

Proxy* ProxyManager::createProxy (int instanceID_) {
    assert (instanceID_);
    boost::mutex::scoped_lock lock (_mutex);
    Proxy* proxy = _moduleManager->createProxy (instanceID_);
    ProxyTable::iterator ptIt = _proxyTable.find (instanceID_);
    if (ptIt != _proxyTable.end ()) {
        ptIt->second.push_back (proxy);
    } else {
        ProxyList pl;
        pl.push_back (proxy);
        _proxyTable[instanceID_] = pl;
    }
    return proxy;
}

int ProxyManager::destroyProxy (Proxy* proxy_) {
    assert (proxy_);
    boost::mutex::scoped_lock lock (_mutex);
    ProxyTable::iterator ptIt = _proxyTable.find (proxy_->getInstanceID ());
    if (ptIt != _proxyTable.end ()) {
        ProxyList::iterator plIt = std::find (ptIt->second.begin (),
            ptIt->second.end (), proxy_);
        if (plIt != ptIt->second.end ()) {
            _moduleManager->destroyProxy (proxy_, proxy_->getInstanceID ());
            ptIt->second.erase (plIt);
        } else {
            return ERR_UNKNOWN_PROXY;
        }
    } else {
        return ERR_UNKNOWN_PROXY;
    }
    return OK;
}

int ProxyManager::destroyProxy (const std::string& pluginName_) {
    boost::mutex::scoped_lock lock (_mutex);

    ProxyTable::iterator ptIt = _proxyTable.begin ();

    for (; ptIt != _proxyTable.end (); ptIt++) {
        bool needClean = false;
        ProxyList::iterator plIt = ptIt->second.begin ();
        for (; plIt != ptIt->second.end (); plIt++) {
            if ((*plIt)->getPluginName() != pluginName_) {
                break;
            } else {
                needClean = true;
                _moduleManager->destroyProxy ((*plIt),
                    (*plIt)->getInstanceID ());
            }
        }
        if (needClean) {
            ptIt->second.clear ();
        }
    }

    return OK;
}

int ProxyManager::destroySourceProxy (const std::string& pluginName_) {
    boost::mutex::scoped_lock lock (_mutex);

    ProxyTable::iterator ptIt = _proxyTable.begin ();

    for (; ptIt != _proxyTable.end (); ptIt++) {
        ProxyList::iterator plIt = ptIt->second.begin ();
        for (; plIt != ptIt->second.end ();) {
            if ((*plIt)->getPluginName() != pluginName_) {
                break;
            } else {
                if ((*plIt)->getSource ())
                {
                    _moduleManager->destroyProxy ((*plIt),
                    (*plIt)->getInstanceID ());
                    ptIt->second.erase(plIt++);
                } else {
                    plIt++;
                }
            }
        }
    }

    return OK;
}

