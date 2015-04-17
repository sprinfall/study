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
 \file message_queue_base.hpp
 \brief the base template class of message queue
 */

#ifndef message_queue_base_hpp
#define message_queue_base_hpp

#include <list>
#include <vector>
#include <algorithm>
#include <climits>
#include <cassert>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/xtime.hpp>
#include "noodle.hpp"

/*!
 \brief the base template class of message queue
 */

template <typename T>
class MessageQueueBase {
public:

    /*!
     \brief constructor
     */

    MessageQueueBase () {}

    /*!
     \brief destructor
     */

    ~MessageQueueBase () {}

    /*!
     \brief put message into message queue
     \param message_ message pointer
     \param urgent_ put message at the front of message queue if true
     \return ok or error code
     */

    int putMessage (T* message_, bool urgent_ = false);

    /*!
     \brief get message from message queue with timeout
     \param utimeOut_ timeout in millionseconds
     \return message pointer
     */

    T* getMessage (int utimeOut_ = 0);

    /*!
     \brief get message count in message queue
     \return ok or error code
     */

    int peekMessage ();

    /*!
     \brief get message from message queue immediately
     \return message pointer
     */

    T* tryGetMessage ();

private:
    typedef std::list<T*> MessageList;

    boost::mutex     _mutex;        ///< binary lock
    boost::condition _condition;    ///< condition
    MessageList      _messageList;  ///< message list

    static const int NANOSECONDS_PER_SECOND = 1000000000; // billion nanosecond
};

template <typename T>
int MessageQueueBase<T>::putMessage (T* message_, bool urgent_) {
    assert (message_);
    bool empty = false;

    boost::mutex::scoped_lock lock (_mutex);

    if (_messageList.empty ()) {
        empty = true;
    }

    if (urgent_) {
        _messageList.push_front (message_);
    } else {
        _messageList.push_back (message_);
    }

    if (empty) {
        _condition.notify_all ();
    }

    return OK;
}

template <typename T>
T* MessageQueueBase<T>::getMessage (int utimeOut_) {
    T* message = 0;
    boost::mutex::scoped_lock lock (_mutex);
    if (_messageList.empty ()) {
        if (!utimeOut_) {
            while (_messageList.empty ()) {
                _condition.wait (lock);
            }
            message = _messageList.front ();
            _messageList.pop_front ();
        } else {
            boost::xtime xt;  
            boost::xtime_get (&xt, boost::TIME_UTC);
            xt.sec  += ((size_t)utimeOut_ * 1000) / NANOSECONDS_PER_SECOND;
            xt.nsec += ((size_t)utimeOut_ * 1000) % NANOSECONDS_PER_SECOND; 
            _condition.timed_wait (lock, xt);
            if (!_messageList.empty ()) {
                message = _messageList.front ();
                _messageList.pop_front ();
            }
        }
    } else {
        message = _messageList.front ();
        _messageList.pop_front ();
    }
    return message;
}

template <typename T>
int MessageQueueBase<T>::peekMessage ()
{
    boost::mutex::scoped_lock lock (_mutex);
    return (int)_messageList.size ();
}

template <typename T>
T* MessageQueueBase<T>::tryGetMessage ()
{
    T* message = 0;
    boost::mutex::scoped_lock lock (_mutex);
    if (!_messageList.empty ()) {
        message = _messageList.front ();
        _messageList.pop_front ();
    }
    return message;
}

#endif // message_queue_base_hpp
