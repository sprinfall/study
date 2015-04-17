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
 \file script.hpp
 \brief Mini-Script
 */

#ifndef script_hpp
#define script_hpp

#include <string>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <map>
#include <list>
#include <sstream>
#include <cmath>
#include <cfloat>

//#define BOOST_SPIRIT_DEBUG // boost spirit debug mode

#include <boost/version.hpp>
#if (BOOST_VERSION >= 103800)
#include <boost/spirit/include/classic_core.hpp>
#include <boost/spirit/include/classic_error_handling.hpp>
#include <boost/spirit/home/classic/iterator/file_iterator.hpp>
#include <boost/spirit/home/classic/iterator/position_iterator.hpp>
#include <boost/spirit/include/classic_attribute.hpp>
#include <boost/spirit/include/classic_utility.hpp>
using namespace boost::spirit::classic;
#else
#include <boost/spirit/core.hpp>
#include <boost/spirit/error_handling.hpp>
#include <boost/spirit/iterator.hpp>
#include <boost/spirit/attribute.hpp>
#include <boost/spirit/utility.hpp>
using namespace boost::spirit;
#endif

#include "script_object.hpp"

static const char* BOOLEAN_TRUE  = "true";
static const char* BOOLEAN_FALSE = "false";

typedef file_iterator<char>            iterator_t;
typedef position_iterator<const char*> position_iterator_t;
typedef std::list<Domain*>             DomainList;
typedef std::map<std::string, Object*> GlobalObjectTable; // reserved

/*!
 \brief Mini-Script
 */

class MiniScript {
private:
    #include "script_detail.ipp"

    std::string       _path;              ///< file path
    std::string       _value;             ///< token value
    std::string       _error;             ///< reserved
    GlobalObjectTable _globalObjectTable; ///< reserved
    DomainList        _domainList;        ///< domain list
    Domain*           _curDomain;         ///< current domain
    Object*           _curObject;         ///< current object
    Object*           _queryObject;       ///< curent query object
    int               _domainLevel;       ///< domain level count

    /*!
     \brief check duplicate object
     \param objectName_ object name
     \return true of false
     */

    bool checkObject (const std::string& objectName_);

    /*!
     \brief check duplicate domain
     \param domainName_ domain name
     \return true of false
     */

    bool checkDomain (const std::string& domainName_);

    /*!
     \brief print object tree
     \param object_ Object pointer
     \param level_ the level of tree
     */

    void printObject (Object* object_, int level_ = 0);

    /*!
     \brief delete object tree
     \param object_ Object pointer
     */

    void deleteObject (Object* object_);

public:

    /*!
     \brief constructor
     \param path_ file path
     */

    MiniScript (const std::string& path_)
        : _path (path_), _curDomain (0), _curObject (0), _domainLevel (0),
          _queryObject (0), _error ("no error") {}

    /*!
     \brief destructor
     */

    ~MiniScript () {
        clean ();
    }

    /*!
     \brief parse script
     \return 0 means fail, 1 means ok
     */

    int     eval ();

    /*!
     \brief release all resources of object
     */

    void    clean ();

    /*!
     \brief print syntax tree for debug
     */

    void    printSyntaxTree ();

    /*!
     \brief query object from script
     \param query_ query string
     \return gotten Object pointer
     */

    Object* getObject (const char* query_);

    /*!
     \brief reserved for future
     */

    inline  std::string const& getError () { // reserved
        return _error;
    }
};

#endif // script_hpp
