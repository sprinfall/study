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
 \file global_object_manager.hpp
 \brief manage the global objects
 */

#ifndef global_object_manager_hpp
#define global_object_manager_hpp

#include <map>
#include <string>
#include <cassert>
#include <stdexcept>

#include "read_write_lock.hpp"
#include "noodle.hpp"

/*!
 \brief the class to manage global objects
 */

class GlobalObjectManager {
public:

    /*!
     \brief get GlobalObjectManager pointer
     \return GlobalObjectManager pointer
     */

    static GlobalObjectManager* getInstance ();

    /*!
     \brief register a global object
     \param name_ the unique global name of object
     \param object_ object pointer
     \return OK or error code, get the error detail by ErrorString::errorString
     */

    int registerObject (const char* name_, void* object_);

    /*!
     \brief unregister a global object
     \param name_ the unique global name of object
     \return OK or error code, get the error detail by ErrorString::errorString
     */

    int unregisterObject (const char* name_);

    /*!
     \brief unregister all global objects
     \return OK or error code, get the error detail by ErrorString::errorString
     */

    int unregisterAll ();

    /*!
     \brief get global object pointer by type
     \param name_ the unique global name of object
     \return global object pointer
     */

    template <typename T>
    T getObject (const char* name_) {
        scope_read_lock (_rwmutex);
        ObjectTable::iterator otIt = _objectTable.find (std::string (name_));
        if (otIt == _objectTable.end ()) {
            throw std::runtime_error ("inexist global object");
        }
        return reinterpret_cast<T>(otIt->second);
    }

    /*!
     \brief get global object pointer
     \param name_ the unique global name of object
     \return global object pointer
     */

    void* getRawObject (const char* name_) {
        scope_read_lock (_rwmutex);
        ObjectTable::iterator otIt = _objectTable.find (std::string (name_));
        if (otIt == _objectTable.end ()) {
            throw std::runtime_error ("inexist global object");
        }
        return otIt->second;
    }

private:
    GlobalObjectManager ()
#if (BOOST_VERSION < 103500)
        : _rwmutex (boost::read_write_scheduling_policy::writer_priority)
#endif
    {}
    GlobalObjectManager (const GlobalObjectManager&);
    GlobalObjectManager& operator= (const GlobalObjectManager&);

private:
    typedef std::map <std::string, void*> ObjectTable;
    ObjectTable _objectTable; ///< object table
#if (BOOST_VERSION >= 103500)
    boost::shared_mutex     _rwmutex; ///< read/write lock
#else
    boost::read_write_mutex _rwmutex; ///< read/write lock
#endif
    static GlobalObjectManager* _globalObjectManager; ///< global object manager object
};

#endif // global_object_manager_hpp
