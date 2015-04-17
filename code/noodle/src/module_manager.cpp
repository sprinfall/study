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

#include <iostream>
#include <exception>

#include "module_manager.hpp"
#include "global_object_manager.hpp"
#include "log.hpp"

ModuleManager::~ModuleManager () {
    ModuleTable::iterator mtIt = _moduleTable.begin ();
    for (; mtIt != _moduleTable.end (); mtIt++) {
        delete mtIt->second;
    }
    _moduleTable.clear ();
    _i2mTable.clear ();
    _instanceTable.clear ();
}

int ModuleManager::installModule (const std::string& name_,
    const std::string& path_, bool source) {
    boost::mutex::scoped_lock lock (_mutex);
    ModuleTable::iterator it = _moduleTable.find (name_);
    if (it != _moduleTable.end ()) {
        return ERR_DUP_MODULE;
    }
    Module* module = 0;
    try {
        module = new Module (name_, path_, source);
    } catch (std::exception& e) {
        OBJECT("log", Log)->writeErrorLog (e.what ());
        return ERR_LOAD_MODULE;
    }
    _moduleTable[name_] = module;
    return OK;
}

int ModuleManager::uninstallModule (const std::string& name_) {
    boost::mutex::scoped_lock lock (_mutex);
    ModuleTable::iterator it = _moduleTable.find (name_);
    if (it == _moduleTable.end ()) {
        return ERR_UNKNOWN_MODULE;
    }

    InstanceTable::iterator iIt = _instanceTable.begin ();
    for (; iIt != _instanceTable.end ();) {
        if (iIt->second->getPluginName () == name_) {
            _instanceTable.erase (iIt++);
        } else {
            iIt++;
        }
    }

    i2mTable::iterator i2mIt = _i2mTable.begin ();
    for (; i2mIt != _i2mTable.end ();) {
        if (i2mIt->second->getModuleName () == name_) {
            _i2mTable.erase (i2mIt++);
        } else {
            i2mIt++;
        }
    }

    delete it->second;
    _moduleTable.erase (it);
    return OK;
}

int ModuleManager::updateModule (const std::string& name_, const std::string& updatedName_,
    const std::string& path_) {
    int res = OK;
    Module* module = 0;
    boost::mutex::scoped_lock lock (_mutex);
    ModuleTable::iterator it = _moduleTable.find (name_);
    if (it == _moduleTable.end ()) {
        return ERR_UNKNOWN_MODULE;
    }
    res = it->second->update (updatedName_, path_);
    if (OK != res) {
        return res;
    }
    module = it->second;
    _moduleTable.erase (it);
    _moduleTable[updatedName_] = module;
    return OK;
}

int ModuleManager::createInstance (const std::string& name_, int id_) {
    boost::mutex::scoped_lock lock (_mutex);
    ModuleTable::iterator mtIt = _moduleTable.find (name_);
    if (mtIt == _moduleTable.end ()) {
        return ERR_UNKNOWN_MODULE;
    }
    i2mTable::iterator i2mIt = _i2mTable.find (id_);
    if (i2mIt != _i2mTable.end ()) {
        return ERR_DUP_INSTANCE;
    }
    Instance* instance = mtIt->second->createInstance (id_);
    _instanceTable.insert (std::make_pair (id_, instance));
    _i2mTable.insert (std::make_pair (id_, mtIt->second));
    return OK;
}

int ModuleManager::destroyInstance (int id_) {
    int res = OK;
    boost::mutex::scoped_lock lock (_mutex);
    i2mTable::iterator i2mIt = _i2mTable.find (id_);
    if (i2mIt == _i2mTable.end ()) {
        return ERR_UNKNOWN_INSTANCE;
    }
    InstanceTable::iterator iIt = _instanceTable.find (id_);
    if (iIt == _instanceTable.end ()) {
        return ERR_UNKNOWN_INSTANCE;
    }
    res = i2mIt->second->destroyInstance (iIt->second);
    _i2mTable.erase (i2mIt);
    _instanceTable.erase (iIt);
    return res;
}

int ModuleManager::disableInstance (int id_) {
    boost::mutex::scoped_lock lock (_mutex);
    InstanceTable::iterator iIt = _instanceTable.find (id_);
    if (iIt == _instanceTable.end ()) {
        return ERR_UNKNOWN_INSTANCE;
    }
    iIt->second->notify (STATE_DISABLE);
    return OK;
}

int ModuleManager::enableInstance (int id_) {
    boost::mutex::scoped_lock lock (_mutex);
    InstanceTable::iterator iIt = _instanceTable.find (id_);
    if (iIt == _instanceTable.end ()) {
        return ERR_UNKNOWN_INSTANCE;
    }
    iIt->second->notify (STATE_ACTIVE);
    return OK;
}

Proxy* ModuleManager::createProxy (int id_) {
    boost::mutex::scoped_lock lock (_mutex);
    InstanceTable::iterator iIt = _instanceTable.find (id_);
    if (iIt == _instanceTable.end ()) {
        throw std::runtime_error ("unknown instance identifier");
    }
    Proxy* proxy = new Proxy (iIt->first,
        iIt->second->getPluginName (), iIt->second->getSource());
    iIt->second->attach (proxy);
    return proxy;
}

int ModuleManager::destroyProxy (Proxy* proxy_, int id_) {
    boost::mutex::scoped_lock lock (_mutex);
    InstanceTable::iterator iIt = _instanceTable.find (id_);
    if (iIt == _instanceTable.end ()) {
        delete proxy_;
        return OK;
    }
    iIt->second->detach (proxy_);
    delete proxy_;
    return OK;
}

bool ModuleManager::sourcable (const std::string& name_) {
    boost::mutex::scoped_lock lock (_mutex);
    ModuleTable::iterator it = _moduleTable.find (name_);
    if (it != _moduleTable.end ()) {
        return it->second->sourcable ();
    }
    return false;
}

int ModuleManager::getRandomInstanceID (const std::string& name_) {
    boost::mutex::scoped_lock lock (_mutex);
    ModuleTable::iterator mtIt = _moduleTable.find (name_);
    if (mtIt == _moduleTable.end ()) {
        return ERR_UNKNOWN_MODULE;
    }
    return mtIt->second->getRandomInstanceID ();
}

void ModuleManager::printTree (std::stringstream& ostream_) {
    boost::mutex::scoped_lock lock (_mutex);
    ModuleTable::iterator mtIt = _moduleTable.begin ();
    for (; mtIt != _moduleTable.end (); mtIt++) {
        ostream_ << mtIt->first << " : "
            << mtIt->second->getFullName () << "\n";
        mtIt->second->printTree (ostream_);
        ostream_ << "\n";
    }
}
