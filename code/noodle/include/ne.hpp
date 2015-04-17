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

#ifndef ne_hpp
#define ne_hpp

#include "noodle.hpp"

class NE {
public:
    NE () {}
    virtual ~NE () {}

    /*!
     \brief create a new message from memory pool
     \param action_ the action of message
     \param station_ the current instance identifier
     \param attachment_ user's attachment need to deliver to another instance
     \param length_ the length of attachment
     \return created message pointer or null means failure
     */

    virtual void* createMessage (int action_, int station_,
        const void* attachment_, int length_) = 0;

    /*!
     \brief post a message to Noodle Engine
     \param message_ message
     \return OK or error code
     */

    virtual int postMessage (void* message_) = 0;

    /*!
     \brief destroy message
     \param message_ message pointer
     */

    virtual void destroyMessage (void* message_) = 0;

    /*!
     \brief get instance identifier of specific plugin
     \param pluginName_ plugin name
     \return instance identifier
     */

    virtual int getInstanceIdentifier (const char* pluginName_) = 0;

    /*!
     \brief set the current instance identifier
     \param message_ message pointer
     \param station_ the current instance identifier
     */

    virtual void setStation (void* message_, int station_) = 0;

    /*!
     \brief get the action of message
     \param message_ message pointer
     \return the action of message
     */

    virtual int getAction (void* message_) = 0;

    /*!
     \brief set the action of message
     \param message_ message pointer
     \param action_ the action of message
     */

    virtual void setAction (void* message_, int action_) = 0;

    /*!
     \brief get attachment of message
     \param message_ message pointer
     \return attachment of message
     */

    virtual void* getAttachment (void* message_) = 0;

    /*!
     \brief set attachment of message
     \param message_ message pointer
     \param attachment_ of message
     \param length_ the length of attachment
     */

    virtual void  setAttachment (void* message_, void* attachment_,
        int length_) = 0;

    /*!
     \brief get the attachment length of message
     \param message_ message pointer
     \return the length of attachment
     */

    virtual int getAttachmentLength (void* message_) = 0;

    /*!
     * \brief allocates memory from memory pool
     * \param size_ size
     * \return memory pointer, zero means fail
     */

    virtual void* alloc (int size_) = 0;

    /*!
     * \brief put memory to memory pool
     * \param size_ size
     * \param m_ memory pointer
     */

    virtual void free (void* m_, int size_) = 0;

    /*!
     \brief get integer value
     \param query_ query string
     \return integer value
     */

    virtual const int getInteger (const char* query_) = 0;

    /*!
     \brief get double value
     \param query_ query string
     \return double value
     */

    virtual const double getDouble (const char* query_) = 0;

    /*!
     \brief get C string value
     \param query_ query string
     \return C string value
     */

    virtual const char* getString (const char* query_) = 0;

    /*!
     \brief get boolean value
     \param query_ query string
     \return boolean value
     */

    virtual const bool getBoolean (const char* query_) = 0;

    /*!
     \brief get the length of array
     \param query_ query string
     \return the length of array
     */

    virtual const int getArrayLength (const char* query_) = 0;

    /*!
     \brief get the integer value of array by index
     \param query_ query string
     \param index_ array index
     \return the integer value
     */

    virtual const int getArrayIntegerByIndex (const char* query_,
        int index_) = 0;

    /*!
     \brief get the double value of array by index
     \param query_ query string
     \param index_ array index
     \return the double value
     */

    virtual const double getArrayDoubleByIndex (const char* query_,
        int index_) = 0;

    /*!
     \brief get the C string value of array by index
     \param query_ query string
     \param index_ array index
     \return the C string value
     */

    virtual const char* getArrayStringByIndex (const char* query_,
        int index_) = 0;

    /*!
     \brief get the boolean value of array by index
     \param query_ query string
     \param index_ array index
     \return the boolean value
     */

    virtual const bool getArrayBooleanByIndex (const char* query_,
        int index_) = 0;

    /*!
     \brief get the length of dictionary
     \param query_ query string
     \return the length of dictionary
     */

    virtual const int getDictLength (const char* query_) = 0;

    /*!
     \brief get the integer key by index in dictionary
     \param query_ query string
     \param index_ index in dictionary
     \return integer key value
     */

    virtual const int getDictIntegerKeyByIndex (const char* query_,
        int index_) = 0;

    /*!
     \brief get the double key by index in dictionary
     \param query_ query string
     \param index_ index in dictionary
     \return double key value
     */

    virtual const double getDictDoubleKeyByIndex (const char* query_,
        int index_) = 0;

    /*!
     \brief get the string key by index in dictionary
     \param query_ query string
     \param index_ index in dictionary
     \return C string key value
     */

    virtual const char* getDictStringKeyByIndex (const char* query_,
        int index_) = 0;

    /*!
     \brief get the boolean key by index in dictionary
     \param query_ query string
     \param index_ index in dictionary
     \return boolean key value
     */

    virtual const bool getDictBooleanKeyByIndex (const char* query_,
        int index_) = 0;

    /*!
     \brief write log
     \param level_ log level
     \param fmt_ fomatted string like fprintf
     */

    virtual void writeLog (int level_, const char* fmt_, ...) = 0;

    /*!
     \brief write log
     \param level_ log level
     \param prefix_ the prefix of log line
     \param fmt_ fomatted string like fprintf
     */

    virtual void writeLog (int level_, int prefix_, const char* fmt_, ...) = 0;

    /*!
     \brief write log
     \param level_ log level
     \param prefix_ the prefix of log line
     \param file_ file name
     \param func_ function name
     \param line_ line number
     \param fmt_ fomatted string like fprintf
     */

    virtual void writeLog (int level_, int prefix_, const char* file_,
        const char* func_, int line_, const char* fmt_, ...) = 0;
};

#endif // ne_hpp
