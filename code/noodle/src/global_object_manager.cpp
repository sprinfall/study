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

#include "global_object_manager.hpp"

GlobalObjectManager* GlobalObjectManager::_globalObjectManager = 0;

int GlobalObjectManager::registerObject (const char* name_, void* object_) {
    assert (name_);
    assert (object_);

    if (!name_ || !object_) {
        return FAIL;
    }

    scope_write_lock (_rwmutex);
    _objectTable[std::string (name_)] = object_;
    return OK;
}

int GlobalObjectManager::unregisterObject (const char* name_) {
    assert (name_);

    if (!name_) {
        return FAIL;
    }

    scope_write_lock (_rwmutex);
    ObjectTable::iterator otIt = _objectTable.find (std::string (name_));
    if (otIt == _objectTable.end ()) {
        return FAIL;
    }
    _objectTable.erase (otIt);

    return OK;
}

int GlobalObjectManager::unregisterAll () {
    scope_write_lock (_rwmutex);
    _objectTable.clear ();
    return OK;
}

GlobalObjectManager* GlobalObjectManager::getInstance () {
    if (_globalObjectManager) {
        return _globalObjectManager;
    }
    return (_globalObjectManager = new GlobalObjectManager);
}
