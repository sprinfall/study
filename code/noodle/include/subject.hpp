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
 \file subject.hpp
 \brief the subject of observer-subject pattern
 */

#ifndef subject_hpp
#define subject_hpp

#include <list>
#include <boost/thread/mutex.hpp>
#include "noodle.hpp"
#include "message.hpp"

class Observer;

/*!
 * \brief the base class of observer-subject pattern
 *
 * 
 */
class Subject {
public:
    /*!
     \brief constructor
     */
    Subject() : _state (STATE_ACTIVE) {}

    /*!
     * \brief copy constructor
     * \param rht_ the subject need to copy
     *
     *  all the observers of rht_ are detached and
     *  attach to new subject
     */
    Subject (Subject& rht_);

    /*!
     * \brief destructor
     *
     * all the observers are detached and the state of
     * them are set to STATE_INACTIVE
     */
    virtual ~Subject () {
        notify (STATE_INACTIVE);
    }

    /*!
     * \brief attach observer
     * \param observer_ observer
     * \retval always return OK
     */
    int attach (Observer* observer_);

    /*!
     * \brief detach observer
     * \param observer_ observer
     * \retval always return OK
     */
    int detach (Observer* observer_);

    /*!
     * \brief detach all observer
     * \retval always return OK
     */
    int detachAll ();

    /*!
     * \brief notify all observers with a state
     * \param state_ \see State
     * \retval always return OK;
     */
    int notify (State state_);

    /*!
     * \brief get observer count
     * \retval the observer count
     */
    int size ();

    /*!
     \brief the purely virtual method to process message
     */
    virtual int putMessage (Message* message_) = 0;

    /*!
     \brief get message from subject
     \return Message pointer
     */

    virtual Message* getMessage () = 0;

    /*!
     \brief the get the state of subject
     \return the state of subject
     */

    inline State getState () {
        return _state;
    }

    /*!
     \brief get state string by state ID
     \param state_ state ID
     \return state string
     */

    inline static const char* getStateString (State state_) {
        switch (state_) {
            case STATE_ACTIVE:
                return "active";
            case STATE_DISABLE:
                return "disable";
            case STATE_INACTIVE:
                return "inactive";
            case STATE_SUSPEND:
                return "suspend";
        }

        return "unknown state";
    }

    friend class Observer;

private:

    /*!
     \brief detach a observer but does not set the state of observer
     \param observer_ observer
     \retval always return OK
     */
    int _detach (Observer* observer_);

private:
    State                _state;        ///< the state of subject
    boost::mutex         _mutex;        ///< binary lock
    std::list<Observer*> _observerList; ///< observer pointer list
};

#endif // subject_hpp
