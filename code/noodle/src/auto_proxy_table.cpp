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

#include <stdexcept>
#include "auto_proxy_table.hpp"

AutoProxyTable::~AutoProxyTable () {
    ProxyTable::iterator ptIt = _proxyTable.begin ();

    // destroy all proxies

    for (; ptIt != _proxyTable.end (); ptIt++) {
        _proxyManager->destroyProxy (ptIt->second);
    }
}

bool AutoProxyTable::has (int id_) {
    return (_proxyTable.find (id_) != _proxyTable.end ());
}

Proxy* AutoProxyTable::operator[] (int id_) {
    ProxyTable::iterator ptIt = _proxyTable.find (id_);
    if (ptIt == _proxyTable.end ()) {

        // create proxy for instance 'id_'

        Proxy* proxy = _proxyManager->createProxy (id_);
        _proxyTable[id_] = proxy;
        return proxy;
    } else {
        if (STATE_INACTIVE == ptIt->second->getState ()) {
            //_proxyManager->destroyProxy (ptIt->second);
            _proxyTable.erase (ptIt);
            throw std::runtime_error ("want to reference a dead instance");
        }
    }

    return ptIt->second;
}

int AutoProxyTable::destroyProxy (int id_) {
    ProxyTable::iterator ptIt = _proxyTable.find (id_);
    if (ptIt == _proxyTable.end ()) {
        return ERR_UNKNOWN_PROXY;
    }

    int error = _proxyManager->destroyProxy (ptIt->second);
    if (OK != error) {
        return error;
    }

    _proxyTable.erase (ptIt);

    return OK;
}

int AutoProxyTable::destroyProxy (const std::string& pluginName_) {
    ProxyTable::iterator ptIt = _proxyTable.begin ();

    for (; ptIt != _proxyTable.end ();) {
        if (pluginName_ == ptIt->second->getPluginName ()) {
            int error = _proxyManager->destroyProxy (ptIt->second);
            if (OK != error) {
                return error;
            }
            _proxyTable.erase (ptIt++);
        } else {
             ptIt++;
        }
    }

    return OK;
}
