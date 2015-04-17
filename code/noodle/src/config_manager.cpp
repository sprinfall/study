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

#include <stdexcept>
#include "config_manager.hpp"

bool ConfigManager::readConfig () {
    return _miniScript.eval () ? true : false;
}

bool ConfigManager::reloadConfig () {
    _miniScript.clean ();
    return readConfig ();
}

const int ConfigManager::getInteger (const char* query_) {
    Object* object = _miniScript.getObject (query_);
    if (!object) {
        throw std::runtime_error ("not found element");
    }
    if (OT_INTEGER == object->getType ()) {
        return dynamic_cast<Integer*>(object)->getValue ();
    }
    throw std::runtime_error ("the object is not integer");
}

const double ConfigManager::getDouble (const char* query_) {
    Object* object = _miniScript.getObject (query_);
    if (!object) {
        throw std::runtime_error ("not found element");
    }
    if (OT_DECIMAL == object->getType ()) {
        return dynamic_cast<Decimal*>(object)->getValue ();
    }
    throw std::runtime_error ("the object is not double");
}

const char* ConfigManager::getString (const char* query_) {
    Object* object = _miniScript.getObject (query_);
    if (!object) {
        throw std::runtime_error ("not found element");
    }
    if (OT_STRING == object->getType ()) {
        return dynamic_cast<String*>(object)->getValue ().c_str ();
    }
    throw std::runtime_error ("the object is not string");
}

const bool ConfigManager::getBoolean (const char* query_) {
    Object* object = _miniScript.getObject (query_);
    if (!object) {
        throw std::runtime_error ("not found element");
    }
    if (OT_BOOLEAN == object->getType ()) {
        return dynamic_cast<Boolean*>(object)->getValue ();
    }
    throw std::runtime_error ("the object is not boolean");
}

const int ConfigManager::getArrayLength (const char* query_) {
    Object* object = _miniScript.getObject (query_);
    if (!object) {
        throw std::runtime_error ("not found element");
    }
    if (OT_ARRAY == object->getType ()) {
        return dynamic_cast<Array*>(object)->getLength ();
    }
    throw std::runtime_error ("the object is not array");
}

const int ConfigManager::getArrayIntegerByIndex (const char* query_, int index_) {
    Object* object = _miniScript.getObject (query_);
    if (!object) {
        throw std::runtime_error ("not found element");
    }
    if (OT_ARRAY == object->getType ()) {
        Object* integerObject = 0;
        try {
            integerObject = dynamic_cast<Array*>(object)->getChildByIndex (index_);
        } catch (std::exception& e) {
            (void) e;
            throw std::runtime_error ("the index is out of range");
        }
        if (OT_INTEGER == integerObject->getType ()) {
            return dynamic_cast<Integer*>(integerObject)->getValue ();
        } else {
            throw std::runtime_error ("the object is not integer");
        }
    }
    throw std::runtime_error ("the object is not array");
}

const double ConfigManager::getArrayDoubleByIndex (const char* query_, int index_) {
    Object* object = _miniScript.getObject (query_);
    if (!object) {
        throw std::runtime_error ("not found element");
    }
    if (OT_ARRAY == object->getType ()) {
        Object* doubleObject = 0;
        try {
            doubleObject = dynamic_cast<Array*>(object)->getChildByIndex (index_);
        } catch (std::exception& e) {
            (void) e;
            throw std::runtime_error ("the index is out of range");
        }
        if (OT_DECIMAL == doubleObject->getType ()) {
            return dynamic_cast<Decimal*>(doubleObject)->getValue ();
        } else {
            throw std::runtime_error ("the object is not double");
        }
    }
    throw std::runtime_error ("the object is not array");
}

const char* ConfigManager::getArrayStringByIndex (const char* query_, int index_) {
    Object* object = _miniScript.getObject (query_);
    if (!object) {
        throw std::runtime_error ("not found element");
    }
    if (OT_ARRAY == object->getType ()) {
        Object* stringObject = 0;
        try {
            stringObject = dynamic_cast<Array*>(object)->getChildByIndex (index_);
        } catch (std::exception& e) {
            (void) e;
            throw std::runtime_error ("the index is out of range");
        }
        if (OT_STRING == stringObject->getType ()) {
            return dynamic_cast<String*>(stringObject)->getValue ().c_str ();
        } else {
            throw std::runtime_error ("the object is not string");
        }
    }
    throw std::runtime_error ("the object is not array");
}

const bool ConfigManager::getArrayBooleanByIndex (const char* query_, int index_) {
    Object* object = _miniScript.getObject (query_);
    if (!object) {
        throw std::runtime_error ("not found element");
    }
    if (OT_ARRAY == object->getType ()) {
        Object* booleanObject = 0;
        try {
            booleanObject = dynamic_cast<Array*>(object)->getChildByIndex (index_);
        } catch (std::exception& e) {
            (void) e;
            throw std::runtime_error ("the index is out of range");
        }
        if (OT_BOOLEAN == booleanObject->getType ()) {
            return dynamic_cast<Boolean*>(booleanObject)->getValue ();
        } else {
            throw std::runtime_error ("the object is not boolean");
        }
    }
    throw std::runtime_error ("the object is not array");
}

const int ConfigManager::getDictLength (const char* query_) {
    Object* object = _miniScript.getObject (query_);
    if (!object) {
        throw std::runtime_error ("not found element");
    }
    if (OT_DICT == object->getType ()) {
        return dynamic_cast<Dict*>(object)->getLength ();
    }
    throw std::runtime_error ("the object is not dictionary");
}

const int ConfigManager::getDictIntegerKeyByIndex (const char* query_, int index_) {
    Object* object = _miniScript.getObject (query_);
    if (!object) {
        throw std::runtime_error ("not found element");
    }
    if (OT_DICT == object->getType ()) {
        Object* kvObject = 0;
        try {
            kvObject = dynamic_cast<Dict*>(object)->getChildByIndex (index_);
            if (OT_INTEGER == dynamic_cast<KeyValue*>(kvObject)->getKey ()->getType ()) {
                return dynamic_cast<Integer*>(
                    dynamic_cast<KeyValue*>(kvObject)->getKey ())->getValue ();
            } else {
                throw std::runtime_error ("the object is not integer");
            }
        } catch (std::exception& e) {
            (void) e;
            throw std::runtime_error ("the index is out of range");
        }
    }
    throw std::runtime_error ("the object is not dictionary");
}

const double ConfigManager::getDictDoubleKeyByIndex (const char* query_, int index_) {
    Object* object = _miniScript.getObject (query_);
    if (!object) {
        throw std::runtime_error ("not found element");
    }
    if (OT_DICT == object->getType ()) {
        Object* kvObject = 0;
        try {
            kvObject = dynamic_cast<Dict*>(object)->getChildByIndex (index_);
            if (OT_DECIMAL == dynamic_cast<KeyValue*>(kvObject)->getKey ()->getType ()) {
                return dynamic_cast<Decimal*>(
                    dynamic_cast<KeyValue*>(kvObject)->getKey ())->getValue ();
            } else {
                throw std::runtime_error ("the object is not double");
            }
        } catch (std::exception& e) {
            (void) e;
            throw std::runtime_error ("the index is out of range");
        }
    }
    throw std::runtime_error ("the object is not dictionary");
}

const char* ConfigManager::getDictStringKeyByIndex (const char* query_ ,int index_) {
    Object* object = _miniScript.getObject (query_);
    if (!object) {
        throw std::runtime_error ("not found element");
    }
    if (OT_DICT == object->getType ()) {
        Object* kvObject = 0;
        try {
            kvObject = dynamic_cast<Dict*>(object)->getChildByIndex (index_);
            if (OT_STRING == dynamic_cast<KeyValue*>(kvObject)->getKey ()->getType ()) {
                return dynamic_cast<String*>(
                    dynamic_cast<KeyValue*>(kvObject)->getKey ())->getValue ().c_str ();
            } else {
                throw std::runtime_error ("the object is not string");
            }
        } catch (std::exception& e) {
            (void) e;
            throw std::runtime_error ("the index is out of range");
        }
    }
    throw std::runtime_error ("the object is not dictionary");
}

const bool ConfigManager::getDictBooleanKeyByIndex (const char* query_, int index_) {
    Object* object = _miniScript.getObject (query_);
    if (!object) {
        throw std::runtime_error ("not found element");
    }
    if (OT_DICT == object->getType ()) {
        Object* kvObject = 0;
        try {
            kvObject = dynamic_cast<Dict*>(object)->getChildByIndex (index_);
            if (OT_BOOLEAN == dynamic_cast<KeyValue*>(kvObject)->getKey ()->getType ()) {
                return dynamic_cast<Boolean*>(
                    dynamic_cast<KeyValue*>(kvObject)->getKey ())->getValue ();
            } else {
                throw std::runtime_error ("the object is not boolean");
            }
        } catch (std::exception& e) {
            (void) e;
            throw std::runtime_error ("the index is out of range");
        }
    }
    throw std::runtime_error ("the object is not dictionary");
}
