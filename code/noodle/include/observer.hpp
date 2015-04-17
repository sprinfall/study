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
 \file observer.hpp
 \brief the observer of observer-subject pattern
 */

#ifndef observer_hpp
#define observer_hpp

#include <list>
#include <cassert>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include "read_write_lock.hpp"
#include "noodle.hpp"
#include "message.hpp"

class Subject;

class Observer {
public:

    /*!
     \brief constructor
     */

    Observer ()
        : _subject (0),
#if (BOOST_VERSION < 103500)
          _rwmutex (boost::read_write_scheduling_policy::writer_priority),
#endif
          _state (STATE_INACTIVE) {}

    /*!
     \brief destructor
     */

    virtual ~Observer ();

    /*!
     \brief update the state of subject
     \param state_ the state of subject
     \return ok or error code
     */

    int update (State state_);

    /*!
     \brief put a message to subject
     \param message_ Message pointer
     \return ok or error code
     */

    int putMessage (Message* message_);

    /*!
     \brief get a message from subject
     \return Message pointer
     */

    Message* getMessage ();

    /*!
     \brief get the state of subject
     \return the state of subject
     */

    inline int getState () {
        scope_read_lock (_rwmutex);
        return _state;
    }

    friend class Subject;

private:
    int _reset (Subject* subject_);
    int _wait ();
    int _signal ();

private:
    boost::mutex            _mutex;   ///< binary mutex
#if (BOOST_VERSION >= 103500)
    boost::shared_mutex     _rwmutex; ///< read/write mutex
#else
    boost::read_write_mutex _rwmutex; ///< read/write mutex
#endif
    boost::condition        _condition; ///< condition variable
    Subject*                _subject;   ///< subject object
    volatile State          _state;     ///< the state of subject
};

#endif // observer_hpp
