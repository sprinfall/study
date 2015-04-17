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

#ifndef read_write_lock_hpp
#define read_write_lock_hpp

/*!
 \file read_write_lock.hpp
 \brief command header for boost read/write lock
 */

#include <boost/version.hpp>
#if (BOOST_VERSION >= 103500)
#include <boost/thread/shared_mutex.hpp>
#include <boost/thread/locks.hpp>
#else
#include <boost/thread/read_write_mutex.hpp>
#endif

#if(BOOST_VERSION >= 103500) 
#   define scope_write_lock(mutex) \
        boost::unique_lock<boost::shared_mutex> swl (mutex);
#else
#   define scope_write_lock(mutex) \
        boost::read_write_mutex::scoped_write_lock swl (mutex);
#endif

#if(BOOST_VERSION >= 103500)
#   define scope_read_lock(mutex) \
        boost::shared_lock<boost::shared_mutex> srl (mutex);
#else
#   define scope_read_lock(mutex) \
        boost::read_write_mutex::scoped_read_lock srl (mutex);
#endif

#endif // read_write_lock_hpp
