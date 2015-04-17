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
#include <algorithm>
#include <cassert>

#include "ne.hpp"
#include "global_object_manager.hpp"
#include "proxy_manager.hpp"
#include "noodle.hpp"
#include "module.hpp"
#include "os_misc.hpp"
#include "error_string.hpp"

Module::Module (const std::string& name_, const std::string& path_,
    bool source_)
        : _name (name_), _path (path_), _source (source_) {
    std::string error("cannot open module ");
    _fullName = _path;

    // load SO

    _so = OS::dlopen (_fullName.c_str ());
    if (!_so) {
        throw std::runtime_error ((error.append (_fullName)).c_str ());
    }

    // get module export point function

    ModuleProc proc = (ModuleProc)OS::dlsym (_so, HOOKPROC);
    if (!proc) {
        error = "cannot find module export point function ";
        throw std::runtime_error ((error.append (_fullName)).c_str ());
    }

    _plugin = proc ();

    // verify export attributes

    if (!_plugin) {
        error = "cannot get module attributes ";
        throw std::runtime_error ((error.append (_fullName)).c_str ());
    }

    if (OK != _plugin->initPlugin (
        GlobalObjectManager::getInstance ()->getObject<NE*>("environment"),
        name_.c_str ())) {
        error = "error when initialize module ";
        throw std::runtime_error ((error.append (_fullName)).c_str ());
    }
}

Module::~Module () {

    // deinitialize all instances and module

    InstanceList::iterator it = _instanceList.begin ();
    for (; it != _instanceList.end (); it++) {
        (*it)->notify (STATE_SUSPEND);

        if (OK != _plugin->deinitInstance ((*it)->getInstanceID ())) {
            std::string error = "error when deinitialize instance ";
            throw std::runtime_error ((error.append (_fullName)).c_str ());
        }

        (*it)->notify (STATE_INACTIVE);

        delete *it;
    }
    _instanceList.clear ();

    // deinitialize module

    std::string error;
    if (OK != _plugin->deinitPlugin (
        GlobalObjectManager::getInstance ()->getObject<NE*>("environment"))) {
        error = "error when deinitialize module ";
        throw std::runtime_error ((error.append (_fullName)).c_str ());
    }

    delete _plugin;
    OS::dlclose (_so);
}

Instance* Module::createInstance (int instanceID_) {

    Instance* instance = new Instance (_plugin, instanceID_,
        _name, _source);

    // instance initialization

    if (OK != _plugin->initInstance (instanceID_)) {
        delete instance;
        std::string error = "error when initialize instance ";
        throw std::runtime_error ((error.append (_fullName)).c_str ());
    }

    _instanceList.push_back (instance);
    return instance;
}

int Module::destroyInstance (Instance* instance_) {
    assert (instance_);
    InstanceList::iterator it = std::find (_instanceList.begin (),
        _instanceList.end (), instance_);
    if (it == _instanceList.end ()) {
        return ERR_UNDEFINED_INSTANCE;
    }

    // deinitialize instance

    (*it)->notify (STATE_SUSPEND);

    if (OK != _plugin->deinitInstance ((*it)->getInstanceID ())) {
        std::string error = "error when deinitialize instance ";
        throw std::runtime_error ((error.append (_fullName)).c_str ());
    }

    (*it)->notify (STATE_INACTIVE);

    delete *it;
    _instanceList.erase (it);
    return OK;
}

int Module::update (const std::string& name_, const std::string& path_) {
    std::string fullName = path_;
    Handle so = OS::dlopen (fullName.c_str ());
    if (!so) {
        return ERR_UNKNOWN_MODULE;
    }

    // verify entry point function

    Plugin* plugin = 0;
    ModuleProc proc = (ModuleProc)OS::dlsym (so, HOOKPROC);
    if (!proc) {
        OS::dlclose (so);
        return ERR_UNDEFINED_MODULE_PROC;
    } else {
        plugin = proc ();
        if (!plugin) {
            OS::dlclose (so);
            return ERR_UNDEFINED_MODULE_PROC;
        }
    }

    std::string error;

    // STATE_SUSPEND all proxies

    InstanceList::iterator iIt = _instanceList.begin ();
    for (; iIt != _instanceList.end (); iIt++) {
        (*iIt)->notify (STATE_SUSPEND);
    }

    // deinitialize all instances

    iIt = _instanceList.begin ();
    try {
        for (; iIt != _instanceList.end (); iIt++) {
            if (OK != _plugin->deinitInstance ((*iIt)->getInstanceID ())) {
                std::string error = "error when deinitialize instance ";
                throw std::runtime_error ((error.append (_fullName)).c_str ());
            }
        }
    } catch (std::exception& e) {
        (void) e;

        delete plugin;
        OS::dlclose (so);

        InstanceList::iterator ilItRecovery = _instanceList.begin ();
        for (; ilItRecovery != iIt; ilItRecovery++) {

            // try to reinitialize the instances
            // no error check

            _plugin->initInstance (
                (*ilItRecovery)->getInstanceID ());
        }

        // notify all proxies to be STATE_ACTIVE

        ilItRecovery = _instanceList.begin ();
        for (; ilItRecovery != _instanceList.end (); ilItRecovery++) {
            (*ilItRecovery)->notify (STATE_ACTIVE);
        }

        throw;
    }
    
    // deinitialize old module

    if (OK != _plugin->deinitPlugin (OBJECT ("environment", NE))) {

        delete plugin;
        OS::dlclose (so);

        // try to reinitialize plugin
        // no error check

        _plugin->initPlugin (OBJECT ("environment", NE), name_.c_str ());

        InstanceList::iterator ilItRecovery = _instanceList.begin ();
        for (; ilItRecovery != _instanceList.end (); ilItRecovery++) {

            // try to reinitialize the instances
            // no error check

            _plugin->initInstance (
                (*ilItRecovery)->getInstanceID ());
        }

        // notify all proxies to be STATE_ACTIVE

        ilItRecovery = _instanceList.begin ();
        for (; ilItRecovery != _instanceList.end (); ilItRecovery++) {
            (*ilItRecovery)->notify (STATE_ACTIVE);
        }

        error = "error when deinitialize module ";
        throw std::runtime_error ((error.append (_fullName)).c_str ());
    }

    // initialize new module

    if (OK != plugin->initPlugin (OBJECT ("environment", NE), name_.c_str ())) {

        delete plugin;
        OS::dlclose (so);

        // try to reinitialize plugin
        // no error check

        _plugin->initPlugin (OBJECT ("environment", NE), name_.c_str ());

        InstanceList::iterator ilItRecovery = _instanceList.begin ();
        for (; ilItRecovery != _instanceList.end (); ilItRecovery++) {

            // try to reinitialize the instances
            // no error check

            _plugin->initInstance (
                (*ilItRecovery)->getInstanceID ());
        }

        // notify all proxies to be STATE_ACTIVE

        ilItRecovery = _instanceList.begin ();
        for (; ilItRecovery != _instanceList.end (); ilItRecovery++) {
            (*ilItRecovery)->notify (STATE_ACTIVE);
        }

        error = "error when initialize module ";
        throw std::runtime_error ((error.append (fullName)).c_str ());
    }

    try {

        // reset all instance

        InstanceList::iterator it = _instanceList.begin ();
        for (; it != _instanceList.end (); it++) {
            (*it)->reset (plugin);

            // send instance initialization message

            if (OK != plugin->initInstance ((*it)->getInstanceID ())) {

                // memory leak here

                std::string error = "error when initialize instance ";
                throw std::runtime_error ((error.append (_fullName)).c_str ());
            }
        }

        // delete old plugin object

        delete _plugin;

        // reset attribute of module

        _name     = name_;
        _fullName = fullName;
        _plugin   = plugin;

        // unload old module

        OS::dlclose (_so);

        _so = so;

        // STATE_ACTIVE all proxies

        it = _instanceList.begin ();
        for (; it != _instanceList.end (); it++) {
            (*it)->notify (STATE_ACTIVE);
        }
    } catch (std::exception& e) {
        (void) e;

        delete plugin;
        OS::dlclose (so);

        // try to reinitialize plugin
        // no error check

        _plugin->initPlugin (OBJECT ("environment", NE), name_.c_str ());

        InstanceList::iterator ilItRecovery = _instanceList.begin ();
        for (; ilItRecovery != _instanceList.end (); ilItRecovery++) {
            (*ilItRecovery)->notify (STATE_SUSPEND);
            (*ilItRecovery)->reset (_plugin);

            // try to reinitialize the instances
            // no error check

            _plugin->initInstance (
                (*ilItRecovery)->getInstanceID ());
        }

        // notify all proxies to be STATE_ACTIVE

        ilItRecovery = _instanceList.begin ();
        for (; ilItRecovery != _instanceList.end (); ilItRecovery++) {
            (*ilItRecovery)->notify (STATE_ACTIVE);
        }
        throw;
    }    

    return OK;
}

int Module::getRandomInstanceID () {

    // the caller should wrap the exception

    if (_instanceList.empty ()) {
        std::string error = "no instance for ";
        throw std::runtime_error ((error.append (_name)).c_str ());
    }

    return _instanceList[OS::getRandom () % _instanceList.size ()]->getInstanceID ();
}

void Module::printTree (std::stringstream& ostream_) {
    InstanceList::iterator ilIt = _instanceList.begin ();
    for (; ilIt != _instanceList.end (); ilIt++) {
        ostream_ << "\t" << (*ilIt)->getInstanceID () <<
            "[" << (*ilIt)->size () << ":" <<
            Subject::getStateString ((*ilIt)->getState()) << "] ";
    }

    if (_instanceList.empty ()) {
        ostream_ << "no instance";
    }
}
