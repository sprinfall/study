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
 \file script.cpp
 \brief implement Mini-Script
 */

#include <stdexcept>
#include <cstring>

#include "script.hpp"

int MiniScript::eval () {
    iterator_t first (_path.c_str ());
    if (!first)
    {
       std::cout << "cannot open config file: '" + _path + "'" << std::endl;
       return 0;
    }
    iterator_t last = first.make_end ();

    parse_info<iterator_t> r;
    SkipParser skip_p = SkipParser ();
    Script script_p = Script (this);

    try {
        r = parse (first, last, script_p, skip_p);

        //printSyntaxTree (); // for debug

    } catch (std::exception& e) {
        if (std::string (e.what ()).size ()) {
            std::cout << e.what () << std::endl;
        }
    }

    return r.hit;
}

Object* MiniScript::getObject (const char* query_) {
    if (!query_) {
        return 0;
    }

    Object*      queryObject = 0;
    parse_info<> r;
    QueryScript  queryScript (this);

    try {
        r = parse (query_, query_ + strlen (query_), queryScript, space_p);
    } catch (std::exception& e) {
        (void) e;
        return 0;
    }

    if (_queryObject && r.full) {
        queryObject = _queryObject;
        _queryObject = 0;
        return queryObject;
    }
    return 0;
}

bool MiniScript::checkObject (const std::string& objectName_) {
    Children::const_iterator cIt = _curDomain->getChildren ().begin ();
    for (; cIt != _curDomain->getChildren ().end (); cIt++) {
        if ((*cIt)->getName () == objectName_) {
            return true;
        }
    }
    return false;
}

bool MiniScript::checkDomain (const std::string& domainName_) {
    DomainList::iterator dlIt = _domainList.begin ();
    for (; dlIt != _domainList.end (); dlIt++) {
        if ((*dlIt)->getName () == domainName_) {
            return true;
        }
    }
    return false;
}

void MiniScript::printSyntaxTree () {
    DomainList::iterator dlIt = _domainList.begin ();
    for (; dlIt != _domainList.end (); dlIt++) {
        printObject (*dlIt);
    }
}

void MiniScript::printObject (Object* object_, int level_) {
    std::string levelString;
    for (int i = 0; i < level_; i++) {
        levelString += '\t';
    }
    switch (object_->getType ()) {
        case OT_DOMAIN: {
            Children::const_iterator cIt = object_->getChildren ().begin ();
            std::cout << levelString << "Domain(" << object_->getName () <<"){" << std::endl;

            for (; cIt != object_->getChildren ().end (); cIt++) {
                printObject (*cIt, level_ + 1);
            }

            std::cout << levelString << "}" << std::endl;
            break;
        }
        case OT_INTEGER: {
            std::cout << levelString << "Integer(" << object_->getName ().c_str () << ":"
                << dynamic_cast<Integer*>(object_)->getValue () << ")" << std::endl;
            break;
        }
        case OT_DECIMAL: {
            std::cout << levelString << "Decimal(" << object_->getName ().c_str () << ":"
                << dynamic_cast<Decimal*>(object_)->getValue () << ")" << std::endl;
            break;
        }
        case OT_STRING: {
            std::cout << levelString << "String(" << object_->getName ().c_str () << ":"
                << dynamic_cast<String*>(object_)->getValue () << ")" << std::endl;
            break;
        }
        case OT_BOOLEAN: {
            std::cout << levelString << "Boolean(" << object_->getName ().c_str () << ":"
                << dynamic_cast<Boolean*>(object_)->getValue () << ")" << std::endl;
            break;
        }
        case OT_ARRAY: {
            std::cout << levelString << "Array(" << object_->getName () << ")[" << std::endl;
            Children::const_iterator cIt = object_->getChildren ().begin ();
            printObject (*cIt, level_ + 1);
            cIt++;
            for (; cIt != object_->getChildren ().end (); cIt++) {
                printObject (*cIt, level_ + 1);
            }
            std::cout << levelString << "]" << std::endl;
            break;
        }
        case OT_KEY_VALUE: {
            KeyValue* kv = dynamic_cast<KeyValue*>(object_);
            std::cout << levelString << "Pair<" << std::endl;
            printObject (kv->getKey (), level_ + 1);
            printObject (kv->getValue (), level_ + 1);
            std::cout << levelString << ">" << std::endl;
            break;
        }
        case OT_DICT: {
            std::cout << levelString << "Dictionary(" << object_->getName () <<")" << ":(" << std::endl;
            Children::const_iterator cIt = object_->getChildren ().begin ();
            for (; cIt != object_->getChildren ().end (); cIt++) {
                printObject (*cIt, level_ + 1);
            }
            std::cout << levelString << ")" << std::endl;
            break;
        }
    }
}

void MiniScript::clean () {
    DomainList::iterator dlIt = _domainList.begin ();
    for (; dlIt != _domainList.end (); dlIt++) {
        deleteObject (*dlIt);
    }

    // clear domain list

    _domainList.clear ();

    //_path        = ""; // donot clean
    _value       = "";
    _error       = "no error"; // reserved
    _curDomain   = 0;
    _curObject   = 0;
    _domainLevel = 0;
    _queryObject = 0;
}

void MiniScript::deleteObject (Object* object_) {
    switch (object_->getType ()) {
        case OT_DOMAIN: {
            Children::const_iterator cIt = object_->getChildren ().begin ();
            for (; cIt != object_->getChildren ().end (); cIt++) {
                deleteObject (*cIt);
            }
            delete object_;
            break;
        }
        case OT_INTEGER:
        case OT_DECIMAL:
        case OT_STRING:
        case OT_BOOLEAN:
            delete object_;
            break;
        case OT_ARRAY: {
            Children::const_iterator cIt = object_->getChildren ().begin ();
            for (; cIt != object_->getChildren ().end (); cIt++) {
                deleteObject (*cIt);
            }
            delete object_;
            break;
        }
        case OT_KEY_VALUE: {
            KeyValue* kv = dynamic_cast<KeyValue*>(object_);
            deleteObject (kv->getKey ());
            deleteObject (kv->getValue ());
            delete object_;
            break;
        }
        case OT_DICT: {
            Children::const_iterator cIt = object_->getChildren ().begin ();
            for (; cIt != object_->getChildren ().end (); cIt++) {
                deleteObject (*cIt);
            }
            delete object_;
            break;
        }
    }
}
