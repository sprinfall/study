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
 \file config_manager.hpp
 \brief manage configuration
 */

#ifndef config_manager_hpp
#define config_manager_hpp

#include <string>
#include "script.hpp"

/*!
 \brief configuration manager
 */

class ConfigManager {
public:

    /*!
     \brief constructor
     \param path_ config file path
     */

    ConfigManager (const char* path_)
        : _path (path_), _miniScript (_path) {}

    /*!
     \brief destructor
     */

    ~ConfigManager () {}

    /*!
     \brief read and parse config file
     \retval true
     \retval false
     */

    bool readConfig ();

    /*!
     \brief cleanup configuration and reload
     \retval true
     \retval false
     */

    bool reloadConfig ();

    /*!
     \brief get integer value
     \param query_ query string
     \return integer value
     */

    const int getInteger (const char* query_);

    /*!
     \brief get double value
     \param query_ query string
     \return double value
     */

    const double getDouble (const char* query_);

    /*!
     \brief get C string value
     \param query_ query string
     \return C string value
     */

    const char* getString (const char* query_);

    /*!
     \brief get boolean value
     \param query_ query string
     \return boolean value
     */

    const bool getBoolean (const char* query_);

    /*!
     \brief get the length of array
     \param query_ query string
     \return the length of array
     */

    const int getArrayLength (const char* query_);

    /*!
     \brief get the integer value of array by index
     \param query_ query string
     \return the integer value
     */

    const int getArrayIntegerByIndex (const char* query_, int index_);

    /*!
     \brief get the double value of array by index
     \param query_ query string
     \param index_ array index
     \return the double value
     */

    const double getArrayDoubleByIndex (const char* query_, int index_);

    /*!
     \brief get the C string value of array by index
     \param query_ query string
     \param index_ array index
     \return the C string value
     */

    const char* getArrayStringByIndex (const char* query_, int index_);

    /*!
     \brief get the boolean value of array by index
     \param query_ query string
     \param index_ array index
     \return the boolean value
     */

    const bool getArrayBooleanByIndex (const char* query_, int index_);

    /*!
     \brief get the length of dictionary
     \param query_ query string
     \return the length of dictionary
     */

    const int getDictLength (const char* query_);

    /*!
     \brief get the integer key by index in dictionary
     \param query_ query string
     \param index_ index in dictionary
     \return integer key value
     */

    const int getDictIntegerKeyByIndex (const char* query_, int index_);

    /*!
     \brief get the double key by index in dictionary
     \param query_ query string
     \param index_ index in dictionary
     \return double key value
     */

    const double getDictDoubleKeyByIndex (const char* query_, int index_);

    /*!
     \brief get the string key by index in dictionary
     \param query_ query string
     \param index_ index in dictionary
     \return C string key value
     */

    const char* getDictStringKeyByIndex (const char* query_ ,int index_);

    /*!
     \brief get the boolean key by index in dictionary
     \param query_ query string
     \param index_ index in dictionary
     \return boolean key value
     */

    const bool getDictBooleanKeyByIndex (const char* query_, int index_);

    /*!
     \brief get config file path
     \return config file path
     */

    inline const std::string& getPath () const {
        return _path;
    }

private:
    std::string _path; ///< config file path
    MiniScript  _miniScript; ///< MiniScript object
};

#endif // config_manager_hpp
