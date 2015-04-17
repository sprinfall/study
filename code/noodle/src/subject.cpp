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
 \file subject.cpp
 \brief implement subject
 */

#include <algorithm>
#include <cassert>

#include "subject.hpp"
#include "observer.hpp"

Subject::Subject (Subject& rht_)
    : _state (STATE_INACTIVE) {
    boost::mutex::scoped_lock lock (rht_._mutex);
    while (!rht_._observerList.empty ()) {

        // deactive observers first and attach to new subject

        std::list<Observer*>::iterator it = rht_._observerList.begin ();
        (*it)->update (STATE_INACTIVE);
        rht_._observerList.erase (it);
        _observerList.push_back (*it);
        (*it)->update (STATE_ACTIVE);
    }
    _state = STATE_ACTIVE;
}

int Subject::attach (Observer* observer_) {
    assert (observer_);
    boost::mutex::scoped_lock lock (_mutex);

    // reset SUBJECT pointer to OBSERVER

    if (OK == observer_->_reset (this)) {
        _observerList.push_back (observer_);
        observer_->update (STATE_ACTIVE);
    }
    return OK;
}

int Subject::detach (Observer* observer_) {
    assert (observer_);
    boost::mutex::scoped_lock lock (_mutex);
    std::list<Observer*>::iterator it = std::find (_observerList.begin (),
        _observerList.end (), observer_);
    if (it != _observerList.end ()) {
        (*it)->update (STATE_INACTIVE);
        _observerList.erase (it);
    }
    return OK;
}

int Subject::detachAll () {
    boost::mutex::scoped_lock lock (_mutex);
    std::list<Observer*>::iterator it = _observerList.begin ();
    for (; it != _observerList.end (); it++) {
        (*it)->update (STATE_INACTIVE);
    }
    _observerList.clear ();
    return OK;
}

int Subject::notify (State state_) {
    bool needClear = false;
    boost::mutex::scoped_lock lock (_mutex);
    std::list<Observer*>::iterator it = _observerList.begin ();
    _state = state_;
    for (; it != _observerList.end (); it++) {
        if (STATE_INACTIVE == state_) {
            (*it)->update (STATE_INACTIVE);
            needClear = true;
        } else {
            (*it)->update (state_);
        }
    }

    // inactived, clear all OBSERVERs

    if (needClear) {
        _observerList.clear ();
    }

    return OK;
}

int Subject::size () {
    boost::mutex::scoped_lock lock (_mutex);
    return (int)_observerList.size ();
}

int Subject::_detach (Observer* observer_) {
    assert (observer_);
    boost::mutex::scoped_lock lock (_mutex);
    std::list<Observer*>::iterator it = std::find (_observerList.begin (),
        _observerList.end (), observer_);
    if (it != _observerList.end ()) {
        _observerList.erase (it);
    }
    return OK;
}
