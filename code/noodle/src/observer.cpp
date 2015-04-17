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

#include "noodle.hpp"
#include "observer.hpp"
#include "subject.hpp"

Observer::~Observer () {
    scope_write_lock(_rwmutex);
    if ((STATE_INACTIVE != _state) && _subject) {
        _subject->_detach (this);
        _state = STATE_INACTIVE;
    }
}

int Observer::update (State state_) {
    scope_write_lock (_rwmutex);

    if (_state == state_) {
        return OK;
    }

    switch (state_) {
    case STATE_ACTIVE:
        switch (_state) {
        case STATE_INACTIVE: // STATE_INACTIVE -> STATE_ACTIVE
        case STATE_DISABLE:  // STATE_DISABLE -> STATE_ACTIVE
            _state = state_;
            break;
        case STATE_SUSPEND: // STATE_SUSPEND -> STATE_ACTIVE
            _state = state_;
            return _signal ();
        }
        break;
    case STATE_INACTIVE:
        switch (_state) {
        case STATE_ACTIVE:  // STATE_ACTIVE -> STATE_INACTIVE
        case STATE_DISABLE: // STATE_DISABLE -> STATE_INACTIVE
        case STATE_SUSPEND: // STATE_SUSPEND -> STATE_INACTIVE
            _state = state_;
            break;
        }
        break;
    case STATE_DISABLE:
        switch (_state) {
        case STATE_INACTIVE: // STATE_INACTIVE -> STATE_DISABLE
        case STATE_ACTIVE:   // STATE_ACTIVE -> STATE_DISABLE
            _state = state_;
            break;
        case STATE_SUSPEND: // STATE_SUSPEND -> STATE_DISABLE
            _state = state_;
            return _signal ();
        }
        break;
    case STATE_SUSPEND:
        switch (_state) {
        case STATE_INACTIVE: // STATE_INACTIVE -> STATE_SUSPEND
        case STATE_ACTIVE:   // STATE_ACTIVE -> STATE_SUSPEND
        case STATE_DISABLE:  // STATE_DISABLE -> STATE_SUSPEND
            _state = state_;
            break;
        }
        break;
    default:
        return ERR_UNDEFINED_STATE;
    }

    return OK;
}

int Observer::putMessage (Message* message_) {
    assert (message_);
    scope_read_lock (_rwmutex);
    switch (_state) {
    case STATE_SUSPEND:
        srl.unlock ();
        _wait ();
        srl.lock ();
        switch (_state) {
        case STATE_ACTIVE:
            return _subject->putMessage (message_);
        case STATE_INACTIVE:
            return ERR_DEAD_SUBJECT;
        case STATE_DISABLE:
            return ERR_DISABLED_SUBJECT;
        }
        break;
    case STATE_ACTIVE:
        return _subject->putMessage (message_);
    case STATE_INACTIVE:
        return ERR_DEAD_SUBJECT;
    case STATE_DISABLE:
        return ERR_DISABLED_SUBJECT;
    }
    return OK;
}

Message* Observer::getMessage ()
{
    scope_read_lock (_rwmutex);
    switch (_state) {
    case STATE_SUSPEND:
        srl.unlock ();
        _wait ();
        srl.lock ();
        switch (_state) {
        case STATE_ACTIVE:
            return _subject->getMessage ();
        case STATE_INACTIVE:
        case STATE_DISABLE:
            break;
        }
        break;
    case STATE_ACTIVE:
        return _subject->getMessage ();
    case STATE_INACTIVE:
    case STATE_DISABLE:
        break;
    }
    return 0;
}

int Observer::_reset (Subject* subject_) {
    assert (subject_);
    scope_write_lock (_rwmutex);
    if (_subject == subject_) return OK;
    if (STATE_INACTIVE != _state) {
        _subject->_detach (this);
        _state = STATE_INACTIVE;
    }
    _subject = subject_;
    return OK;
}

int Observer::_wait () {
    boost::mutex::scoped_lock lock (_mutex);
    while (STATE_SUSPEND == _state) {
        _condition.wait (lock);
    }
    return OK;
}

int Observer::_signal () {
    boost::mutex::scoped_lock lock (_mutex);
    _condition.notify_all ();
    return OK;
}
