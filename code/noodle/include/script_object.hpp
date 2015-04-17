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
 \file script_object.hpp
 \brief the supported object of Mini-Script
 */

#ifndef script_object_hpp
#define script_object_hpp

#include <string>
#include <vector>
#include <cassert>
#include <iostream>

static const int OT_NONE      = 0; ///< undefined
static const int OT_DOMAIN    = 1; ///< domain object
static const int OT_INTEGER   = 2; ///< integer object
static const int OT_DECIMAL   = 3; ///< floating object
static const int OT_STRING    = 4; ///< string object
static const int OT_DICT      = 5; ///< dictionary object
static const int OT_ARRAY     = 6; ///< array object
static const int OT_KEY_VALUE = 7; ///< key-value object of dictionary
static const int OT_BOOLEAN   = 8; ///< boolean object

class Object;
typedef std::vector<Object*> Children;

/*!
 \brief the base class of Mini-Script object
 */

class Object {
public:

    /*!
     \brief constructor
     \param name_ object name
     \param type_ object type
     \param father_ the father of object
     */

    Object (const char* name_, int type_, Object* father_)
        : _type (type_), _father (father_) {
        if (name_) {
            _name.assign (name_);
        }
    }

    /*!
     \brief destructor
     */

    virtual ~Object () {}

    /*!
     \brief get object type
     \return object type
     */

    inline int getType () {
        return _type;
    }

    /*!
     \brief add a child object to father object
     \param child_ child object
     \return always 0
     */

    inline int addChild (Object* child_) {
        assert (child_);
        _children.push_back (child_);
        return 0;
    }

    /*!
     \brief get father of object
     \return father object
     */

    inline Object* getFather () {
        return _father;
    }

    /*!
     \brief get object name
     \return object name
     */

    inline const std::string& getName () {
        return _name;
    }

    /*!
     \brief check whether is a anonymouse object
     \return true or false
     */

    inline bool isUnamed () {
        return _name.empty ();
    }

    /*!
     \brief get children list of object
     \return the const reference of children list
     */

    inline const Children& getChildren () {
        return _children;
    }

    /*!
     \brief get the count of children
     \return children count
     */

    inline const int getSize () {
        return (int)_children.size ();
    }

private:
    int         _type;     ///< object type
    std::string _name;     ///< object name
    Object*     _father;   ///< father
    Children    _children; ///< children
};

/*!
 \brief domain object
 */

class Domain : public Object {
public:

    /*!
     \brief constructor
     \param name_ object name
     \param father_ the father of object
     */

    Domain (const char* name_, Object* father_ = 0)
        : Object (name_, OT_DOMAIN, father_) {}

    /*!
     \brief destructor
     */

    virtual ~Domain () {}

private:
    std::string _name; ///< domain name, reserved
};

/*!
 \brief integer object
 */

class Integer : public Object {
public:

    /*!
     \brief constructor
     \param name_ object name
     \param value_ integer value
     \param father_ the father of object
     */

    Integer (const char* name_, int value_, Object* father_ = 0)
        : Object (name_, OT_INTEGER, father_),
          _value (value_) {}

    /*!
     \brief destructor
     */

    virtual ~Integer () {}

    /*!
     \brief get integer value
     \return integer value
     */

    inline const int getValue () {
        return _value;
    }

private:
    int _value; ///< integer value
};

/*!
 \brief floating object
 */

class Decimal : public Object {
public:

    /*!
     \brief constructor
     \param name_ object name
     \param value_ double value
     \param father_ the father of object
     */

    Decimal (const char* name_, double value_, Object* father_ = 0)
        : Object (name_, OT_DECIMAL, father_),
          _value (value_) {}

    /*!
     \brief destructor
     */

    virtual ~Decimal () {}

    /*!
     \brief get double value
     \return double value
     */

    inline const double getValue () {
        return _value;
    }

private:
    double _value; ///< double value
};

/*!
 \brief string object
 */

class String : public Object {
public:

    /*!
     \brief constructor
     \param name_ object name
     \param value_ string value
     \param father_ the father of object
     */

    String (const char* name_, const std::string& value_, Object* father_ = 0)
        : Object (name_, OT_STRING, father_),
          _value (value_) {}

    /*!
     \brief destructor
     */

    virtual ~String () {}

    /*!
     \brief get string value
     \return string value
     */

    inline const std::string& getValue () {
        return _value;
    }

private:
    std::string _value; ///< string value
};

/*!
 \brief key-value object for dictionary
 */

class KeyValue : public Object {
public:

    /*!
     \brief constructor
     \param father_ the father of object
     */

    KeyValue (Object* father_ = 0)
        : Object (0, OT_KEY_VALUE, father_) {}

    /*!
     \brief destructor
     */

    virtual ~KeyValue () {}

    /*!
     \brief set the key object key-value
     \param key_ key object
     \return always 0
     */    

    inline int setKey (Object* key_) {
        _key = key_;
        return 0;
    }

    /*!
     \brief set the value object key-value
     \param value_ value object
     \return always 0
     */ 

    inline int setValue (Object* value_) {
        _value = value_;
        return 0;
    }

    /*!
     \brief get key object key-value
     \return key object
     */ 

    inline Object* getKey () {
        return _key;
    }

    /*!
     \brief get value object key-value
     \return value object
     */ 

    inline Object* getValue () {
        return _value;
    }

private:
    Object* _key;   ///< key
    Object* _value; ///< value
};

/*!
 \brief dictionary object

        1. i donot use hashmap to implement this object
        because efficiency for config file is not critical issues
        2. use the same facilities reducing the complexity of code lines
 */

class Dict : public Object {
public:

    /*!
     \brief constructor
     \param name_ name
     \param father_ the father of object
     */

    Dict (const char* name_, Object* father_ = 0)
        : Object (name_, OT_DICT, father_) {}

    /*!
     \brief destructor
     */

    virtual ~Dict () {}

    /*!
     \brief get the length of dictionary
     \return the length of dictionary
     */

    inline const int getLength () {
        return getSize ();
    }

    /*!
     \brief get child object by index
     \param index_ child index
     \return child object
     */

    Object* getChildByIndex (int index_) {
        return getChildren ()[index_];
    }
};

/*!
 \brief array
 */

class Array : public Object {
public:

    /*!
     \brief constructor
     \param name_ name
     \param father_ the father of object
     */

    Array (const char* name_, Object* father_ = 0)
        : Object (name_, OT_ARRAY, father_) {}

    /*!
     \brief destructor
     */

    virtual ~Array () {}

    /*!
     \brief get the length of array
     \return the length of array
     */

    inline const int getLength () {
        return getSize ();
    }

    /*!
     \brief get child object by index
     \param index_ child index
     \return child object
     */

    Object* getChildByIndex (int index_) {
        return getChildren ()[index_];
    }
};

/*!
 \brief boolean
 */

class Boolean : public Object {
public:

    /*!
     \brief constructor
     \param name_ name
     \param value_ boolean value
     \param father_ the father of object
     */

    Boolean (const char* name_, bool value_, Object* father_ = 0)
        : Object (name_, OT_BOOLEAN, father_), _value (value_) {}

    /*!
     \brief destructor
     */

    virtual ~Boolean () {}

    /*!
     \brief get boolean value
     \return boolean value
     */

    inline const bool getValue () {
        return _value;
    }

private:
    bool _value; ///< value
};

#endif // script_object_hpp
