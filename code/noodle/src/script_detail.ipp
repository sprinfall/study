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

struct RestoreFatherDomain {
    void operator()(const char placeHolder_) const 
    { 
        if (_script->_curDomain->getFather ()) {
            _script->_curDomain =
                dynamic_cast<Domain*>(_script->_curDomain->getFather ());
        }
    } 
    RestoreFatherDomain(MiniScript* script_) 
        :_script(script_) {} 
private: 
    MiniScript* _script;
};

struct DecDomainLevel {
    void operator()(const char placeHolder_) const 
    { 
        _script->_domainLevel--;
    } 
    DecDomainLevel(MiniScript* script_) 
        :_script(script_) {} 
private: 
    MiniScript* _script;
};

struct IncDomainLevel {
    void operator()(const char placeHolder_) const 
    { 
        _script->_domainLevel++;
    } 
    IncDomainLevel(MiniScript* script_) 
        :_script(script_) {} 
private: 
    MiniScript* _script;
};

struct CreateInteger {
    void operator()(const int value_) const 
    {
        _script->_curDomain->addChild (
            new Integer (_script->_value.c_str (), value_,
            _script->_curDomain));
        _script->_value.clear ();
    } 
    CreateInteger(MiniScript* script_) 
        :_script(script_) {} 
private: 
    MiniScript* _script;
};

struct CreateDecimal {
    void operator()(const double value_) const 
    {
        _script->_curDomain->addChild (
            new Decimal (_script->_value.c_str (), value_,
            _script->_curDomain));
        _script->_value.clear ();
    } 
    CreateDecimal(MiniScript* script_) 
        :_script(script_) {} 
private: 
    MiniScript* _script;
};

struct CreateString {
    void operator()(iterator_t nameStart_, iterator_t nameEnd_) const
    {
        _script->_curDomain->addChild (
            new String (_script->_value.c_str (),
            std::string (nameStart_ + 1, nameEnd_ - 1),
            _script->_curDomain));
        _script->_value.clear ();
    } 
    CreateString(MiniScript* script_) 
        :_script(script_) {} 
private: 
    MiniScript* _script;
};

struct CreateBoolean {
    void operator()(iterator_t nameStart_, iterator_t nameEnd_) const
    {
        _script->_curDomain->addChild (
            new Boolean (_script->_value.c_str (),
            (std::string (nameStart_, nameEnd_) == BOOLEAN_TRUE ? true : false),
            _script->_curDomain));
        _script->_value.clear ();
    } 
    CreateBoolean(MiniScript* script_) 
        :_script(script_) {} 
private: 
    MiniScript* _script;
};

template <typename T>
struct CreateDictKeyValueGeneric {
    void operator()(const T placeHolder_) const 
    {
        KeyValue* keyValue = new KeyValue (_script->_curObject);
        dynamic_cast<Dict*>(_script->_curObject)->addChild (keyValue); // add to father dict
        _script->_curObject = keyValue; // set self to current object
    } 
    CreateDictKeyValueGeneric (MiniScript* script_) 
        :_script(script_) {} 
private: 
    MiniScript* _script;
};

struct CreateDictKeyValueRange {
    void operator()(iterator_t placeHolder1_, iterator_t placeHolder2_) const 
    {
        KeyValue* keyValue = new KeyValue (_script->_curObject);
        dynamic_cast<Dict*>(_script->_curObject)->addChild (keyValue); // add to father dict
        _script->_curObject = keyValue; // set self to current object
    } 
    CreateDictKeyValueRange (MiniScript* script_) 
        :_script(script_) {} 
private: 
    MiniScript* _script;
};

struct CreateDictIntegerKey {
    void operator()(const int _value) const 
    { 
        Integer* integer = new Integer (0, _value, _script->_curObject);
        dynamic_cast<KeyValue*>(_script->_curObject)->setKey (integer);
    } 
    CreateDictIntegerKey(MiniScript* script_) 
        :_script(script_) {} 
private: 
    MiniScript* _script;
};

struct CreateDictDecimalKey {
    void operator()(const double _value) const 
    { 
        Decimal* decimal = new Decimal (0, _value, _script->_curObject);
        dynamic_cast<KeyValue*>(_script->_curObject)->setKey (decimal);
    } 
    CreateDictDecimalKey(MiniScript* script_) 
        :_script(script_) {} 
private: 
    MiniScript* _script;
};

struct CreateDictStringKey {
    void operator()(iterator_t nameStart_, iterator_t nameEnd_) const 
    { 
        String* string =
            new String (0, std::string (nameStart_ + 1, nameEnd_ - 1),
            _script->_curObject);
        dynamic_cast<KeyValue*>(_script->_curObject)->setKey (string);
    } 
    CreateDictStringKey (MiniScript* script_) 
        :_script(script_) {} 
private: 
    MiniScript* _script;
};

struct CreateDictBooleanKey {
    void operator()(iterator_t nameStart_, iterator_t nameEnd_) const 
    { 
        Boolean* boolean =
            new Boolean (
                0, 
                (std::string (nameStart_, nameEnd_) == BOOLEAN_TRUE ? true : false),
                _script->_curObject);
        dynamic_cast<KeyValue*>(_script->_curObject)->setKey (boolean);
    } 
    CreateDictBooleanKey (MiniScript* script_) 
        :_script(script_) {} 
private: 
    MiniScript* _script;
};

struct CreateDictIntegerValue {
    void operator()(const int _value) const 
    { 
        Integer* integer = new Integer (0, _value, _script->_curObject);
        dynamic_cast<KeyValue*>(_script->_curObject)->setValue (integer);
    } 
    CreateDictIntegerValue(MiniScript* script_) 
        :_script(script_) {} 
private: 
    MiniScript* _script;
};

struct CreateDictDecimalValue {
    void operator()(const double _value) const 
    { 
        Decimal* decimal = new Decimal (0, _value, _script->_curObject);
        dynamic_cast<KeyValue*>(_script->_curObject)->setValue (decimal);
    } 
    CreateDictDecimalValue(MiniScript* script_) 
        :_script(script_) {} 
private: 
    MiniScript* _script;
};

struct CreateDictStringValue {
    void operator()(iterator_t nameStart_, iterator_t nameEnd_) const 
    { 
        String* string =
            new String (0, std::string (nameStart_ + 1, nameEnd_ - 1),
            _script->_curObject);
        dynamic_cast<KeyValue*>(_script->_curObject)->setValue (string);
    } 
    CreateDictStringValue (MiniScript* script_) 
        :_script(script_) {} 
private: 
    MiniScript* _script;
};

struct CreateDictBooleanValue {
    void operator()(iterator_t nameStart_, iterator_t nameEnd_) const 
    { 
        Boolean* boolean =
            new Boolean (
                0,
                (std::string (nameStart_, nameEnd_) == BOOLEAN_TRUE ? true : false),
                _script->_curObject);
        dynamic_cast<KeyValue*>(_script->_curObject)->setValue (boolean);
    } 
    CreateDictBooleanValue (MiniScript* script_) 
        :_script(script_) {} 
private: 
    MiniScript* _script;
};

struct CreateDict {
    void operator()(const char placeHolder_) const
    {
        Object* object = 0;
        if (!_script->_value.empty ()) { // the dict value in key = value
            _script->_curDomain->addChild (
                object = new Dict (_script->_value.c_str (),
                _script->_curDomain));
            _script->_value.clear ();
            _script->_curObject = object;
        } else {
            // for dict value of key => value
            object = new Dict (0, _script->_curObject);
            if (OT_KEY_VALUE == _script->_curObject->getType ()) {
                dynamic_cast<KeyValue*>(_script->_curObject)->setValue (object);
            } else {
                _script->_curObject->addChild (object);
            }
            _script->_curObject = object;
        }
    } 
    CreateDict(MiniScript* script_) 
        :_script(script_) {} 
private: 
    MiniScript* _script;
};

struct Restore {
    void operator()(iterator_t placeHolder1_, iterator_t placeHolder2_) const 
    {
        _script->_curObject = _script->_curObject->getFather ();
    } 
    Restore(MiniScript* script_) 
        :_script(script_) {} 
private: 
    MiniScript* _script;
};

struct CreateArray {
    void operator()(const char placeHolder_) const
    {
        Object* object = 0;
        if (!_script->_value.empty ()) {
            _script->_curDomain->addChild (object =
                new Array (_script->_value.c_str (), _script->_curDomain));
            _script->_value.clear ();
            _script->_curObject = object;
        } else {
            object = new Array (0, _script->_curObject);
            if (OT_KEY_VALUE == _script->_curObject->getType ()) {
                dynamic_cast<KeyValue*>(_script->_curObject)->setValue (object);            
            } else {
                _script->_curObject->addChild (object);
            }
            _script->_curObject = object;
        }
    } 
    CreateArray(MiniScript* script_) 
        :_script(script_) {} 
private: 
    MiniScript* _script;
};

struct CheckDupObjectName {
    void operator()(const char placeHolder_) const
    {
        if (_script->checkObject (_script->_value)) {
            throw std::runtime_error (
                "Duplicate object '" + _script->_value + "' in domain '" +
                _script->_curDomain->getName () + "'");
        }
    } 
    CheckDupObjectName(MiniScript* script_) 
        :_script(script_) {} 
private: 
    MiniScript* _script;
};

struct CheckDupObjectDomainName {
    void operator()(iterator_t placeHolder1_, iterator_t placeHolder2_) const
    {
        if (_script->checkDomain (_script->_value)) {
            throw std::runtime_error ("Duplicate domain '" +
                _script->_value + "'");
        }
    } 
    CheckDupObjectDomainName (MiniScript* script_) 
        :_script(script_) {} 
private: 
    MiniScript* _script;
};

struct CreateArrayIntegerChild {
    void operator()(const int value_) const
    {
        _script->_curObject->addChild (
            new Integer (0, value_, _script->_curObject));
    } 
    CreateArrayIntegerChild(MiniScript* script_) 
        :_script(script_) {} 
private: 
    MiniScript* _script;
};

struct CreateArrayDecimalChild {
    void operator()(const double value_) const
    {
        _script->_curObject->addChild (
            new Decimal (0, value_, _script->_curObject));
    } 
    CreateArrayDecimalChild(MiniScript* script_) 
        :_script(script_) {} 
private: 
    MiniScript* _script;
};

struct CreateArrayStringChild {
    void operator()(iterator_t nameStart_, iterator_t nameEnd_) const
    {
        _script->_curObject->addChild (
            new String (0, std::string (nameStart_ + 1, nameEnd_ - 1),
            _script->_curObject));
    } 
    CreateArrayStringChild(MiniScript* script_) 
        :_script(script_) {} 
private: 
    MiniScript* _script;
};

struct CreateArrayBooleanChild {
    void operator()(iterator_t nameStart_, iterator_t nameEnd_) const
    {
        _script->_curObject->addChild (
            new Boolean (
                0,
                (std::string (nameStart_, nameEnd_) == BOOLEAN_TRUE ? true : false),
                _script->_curObject));
    } 
    CreateArrayBooleanChild(MiniScript* script_) 
        :_script(script_) {} 
private: 
    MiniScript* _script;
};

struct CreateDomain {
    void operator()(iterator_t nameStart_, iterator_t nameEnd_) const 
    { 
        if (_script->_domainList.empty ()) {
            Domain* domain =
                new Domain (std::string(nameStart_, nameEnd_).c_str (), 0);
            _script->_domainList.push_back (domain);
            _script->_curDomain = domain;
        } else {            
            if (_script->_domainLevel) {
                Domain* domain =
                    new Domain (std::string(nameStart_, nameEnd_).c_str (),
                    _script->_curDomain);
                _script->_curDomain->addChild (domain);
                _script->_curDomain = domain;
            } else {
                Domain* domain =
                    new Domain (
                        std::string(nameStart_, nameEnd_).c_str (),
                        0);
                _script->_domainList.push_back (domain);
                _script->_curDomain = domain;
            }
        }
    } 
    CreateDomain(MiniScript* script_) 
        :_script(script_) {} 
private: 
    MiniScript* _script;
};

struct CreateDomainAfter {
    void operator()(const char placeHolder_) const 
    {
        Domain* domain =
            new Domain (_script->_value.c_str (), _script->_curDomain);
        _script->_value.clear ();
        _script->_curDomain->addChild (domain);
        _script->_curDomain = domain;
    } 
    CreateDomainAfter (MiniScript* script_) 
        :_script(script_) {} 
private: 
    MiniScript* _script;
};

struct SetValue {
    void operator()(iterator_t nameStart_, iterator_t nameEnd_) const 
    { 
        _script->_value.assign (nameStart_, nameEnd_); 
    } 
    SetValue(MiniScript* script_) 
        :_script(script_) {} 
private: 
    MiniScript* _script;
};

struct SkipParser : public grammar<SkipParser> {
    template <typename ScannerT> struct definition {
        rule<ScannerT> skip_p;
        definition(SkipParser const& self) {
            skip_p =
                space_p
                | comment_p("#")
                | comment_p("--")
                | comment_p("//")
                | comment_p("/*", "*/")
                ;
        }
        rule<ScannerT> const& start() const {
            return skip_p;
        }
    };
};

enum ParserError {
    need_left_brace = 1,
    need_right_brace = 2,
    need_comma,
    need_right_round,
    need_left_round,
    need_pointer,
    need_identifier
};

struct ErrorHandler
{
    ErrorHandler (MiniScript* script_ = 0)
        : _script (script_) {
    }
    template <typename ScannerT, typename ErrorT>
    error_status<> operator()(ScannerT const& scan, ErrorT const& error) const
    {
        //file_position fpos = scan.first.get_position();
        //std::cout << "line:" << fpos.line << ":";
        switch (error.descriptor) {
        case need_left_brace:
            std::cout << "need a '{'" << std::endl;
            break;
        case need_right_brace:
            std::cout << "need a '}'" << std::endl;
        default:
            std::cout << "internal error" << std::endl;
            break;
        }

        return error_status<>(error_status<>::accept);
    }

    MiniScript* _script;
};


struct ErrorReportMsg {
    char const *error_msg;

    ErrorReportMsg(const char *msg_)
        : error_msg(msg_) {}

    typedef nil_t result_t;

    template <typename ScannerT> int
        operator()(ScannerT const& scan, result_t& result) const {
            if (scan.at_end()) {
            } else {
                //file_position fpos = scan.first.get_position();
                std::stringstream error;
                //error << "line:" << fpos.line << ":" << error_msg;
                error << error_msg;
                throw std::runtime_error (error.str ().c_str ());
            }
            return -1;
    }
};

typedef functor_parser<ErrorReportMsg> error_report_p;

struct Script : public grammar<Script> {
public:
    Script (MiniScript* miniScript_) : _miniScript (miniScript_) {}
    MiniScript* _miniScript;

    template <typename ScannerT> struct definition {

        rule<ScannerT> script, domainBody, domain, subDomain;
        rule<ScannerT> keyValue, value, identifier;
        rule<ScannerT> string, decimal, integer, dict, array;
        rule<ScannerT> singleQuoteString, doubleQuoteString;
        rule<ScannerT> dictKeyVal, dictKey, dictVal, arrayElemList;
        rule<ScannerT> globalVar, arrayElem, boolean;
        rule<ScannerT> attributeGet;

        definition(Script const& self) {
            assertion<ParserError> needLeftBrace(need_left_brace);
            assertion<ParserError> needRightBrace(need_right_brace);
            guard<ParserError> parserGuard;

            script =
                *(     
                    domain
                    | error_report_p ("invalid domain")
                )
                ;

            domain =
                parserGuard
                (
                    identifier
                    [
                        SetValue (self._miniScript)
                    ]
                    [
                        CheckDupObjectDomainName (self._miniScript)
                    ]
                    [
                        CreateDomain (self._miniScript)
                    ]
                    >>
                    needLeftBrace
                    (
                        ch_p('{')[IncDomainLevel (self._miniScript)]
                    )
                    >>
                    domainBody
                    >>
                    needRightBrace
                    (
                        ch_p('}')[DecDomainLevel (self._miniScript)]
                    )
                    >>
                    !ch_p(';')
                )           
                [
                    ErrorHandler()
                ] 
                ;

            domainBody = 
                *(
                    identifier
                    [
                        SetValue (self._miniScript)
                    ]
                    >>
                    (
                        (
                            ch_p('=')
                            [
                                CheckDupObjectName (self._miniScript)
                            ]
                            >> keyValue
                        )
                        |
                        (
                            ch_p('{')
                            [
                                CheckDupObjectName (self._miniScript)
                            ]
                            [
                                IncDomainLevel (self._miniScript)
                            ]
                            [
                                CreateDomainAfter (self._miniScript)
                            ]
                            >>
                            subDomain
                        )
                    )
                )
                ;

            keyValue =
                value
                >>
                !ch_p(';')
                ;

            subDomain =
                parserGuard
                (
                    domainBody
                    >>
                    needRightBrace
                    (
                        ch_p('}')
                        [
                            DecDomainLevel (self._miniScript)
                        ]
                        [
							RestoreFatherDomain (self._miniScript)
                        ]
                    )
                )
                [
                    ErrorHandler()
                ]
                >>
                !ch_p(';')
                ;

            identifier =
                lexeme_d
                [
                    (
                        (
                            range_p('a', 'z')
                            | range_p('A', 'Z')
                            | ch_p('_')
                        )
                        >>
                        *(
                            range_p('0', '9')
                            | range_p('a', 'z')
                            | range_p('A', 'Z')
                            | ch_p('_')
                        )
                    )
                ]
                ;

            attributeGet =
                identifier
                >>
                (
                    *(
                         ch_p('.')
                         >> identifier
                     )
                     >>
                    *(
                         ch_p('[')
                         >>
                         (
                             longest_d
                             [
                                 strict_real_p
                                 | string
                                 | int_p
                             ]
                         )
                         >>
                         ch_p(']')
                     )
                 )
                 >>
                 !ch_p(';')
                 ;

            value =
                (
                    strict_real_p
                    [
                        CreateDecimal (self._miniScript)
                    ]
                    | int_p
                    [
                        CreateInteger (self._miniScript)
                    ]
                    | string
                    [
                        CreateString (self._miniScript)
                    ]
                    | boolean
                    [
                        CreateBoolean (self._miniScript)
                    ]
                    | dict
                    | array
                    //| globalVar
                    //| attributeGet
                )
                | error_report_p ("invalid value")
                ;

            boolean =
                str_p (BOOLEAN_FALSE)
                | BOOLEAN_TRUE
                ;

            string =
                singleQuoteString
                | doubleQuoteString;

            singleQuoteString =
                confix_p('\'', *lex_escape_ch_p, '\'')
                ;

            doubleQuoteString =
                confix_p('"', *lex_escape_ch_p, '"')
                ;

            globalVar =
                ch_p('$')
                >> identifier
                ;

            dict =
                ch_p('(')
                [
                    CreateDict (self._miniScript)
                ]
                >> !dictKeyVal
                >> ch_p(')')
                ;

            dictKeyVal =
                dictKey
                >> str_p("=>")
                >> dictVal
                [
                    Restore (self._miniScript)
                ] >> *(
                ch_p(',')
                >> dictKey
                >> str_p("=>")
                >> dictVal
                [
                    Restore (self._miniScript)
                ]
                );

            dictKey =
                strict_real_p
                [
                    CreateDictKeyValueGeneric<double> (self._miniScript)
                ]
                [
                    CreateDictDecimalKey (self._miniScript)
                ]
                | int_p
                [
                    CreateDictKeyValueGeneric<int> (self._miniScript)
                ]
                [
                    CreateDictIntegerKey (self._miniScript)
                ]
                | string
                [
                    CreateDictKeyValueRange (self._miniScript)
                ]
                [
                    CreateDictStringKey (self._miniScript)
                ]
                | boolean
                [
                    CreateDictKeyValueRange (self._miniScript)
                ]
                [
                    CreateDictBooleanKey (self._miniScript)
                ]
                //| globalVar
                //| attributeGet // WARNING: conflict with boolean grammar
                ;

            dictVal =
                strict_real_p
                [
                    CreateDictDecimalValue (self._miniScript)
                ]
                | int_p
                [
                    CreateDictIntegerValue (self._miniScript)
                ]
                | string
                [
                    CreateDictStringValue (self._miniScript)
                ]
                | boolean
                [
                    CreateDictBooleanValue (self._miniScript)
                ]
                | dict
                [
                    Restore (self._miniScript)
                ]
                | array
                [
                    Restore (self._miniScript)
                ]
                //| globalVar
                //| attributeGet
                ;

            array =
                ch_p('[')
                [
                    CreateArray (self._miniScript)
                ]
                >> !arrayElemList
                >> ch_p(']')
                ;

            arrayElemList =
                arrayElem
                >> *(ch_p (',')
                >> arrayElem)
                ;

            arrayElem =
                strict_real_p
                [
                    CreateArrayDecimalChild (self._miniScript)
                ]
                | int_p
                [
                    CreateArrayIntegerChild (self._miniScript)
                ]
                | string
                [
                    CreateArrayStringChild (self._miniScript)
                ]
                | boolean
                [
                    CreateArrayBooleanChild (self._miniScript)
                ]
                | dict
                [
                    Restore (self._miniScript)
                ]
                | array
                [
                    Restore (self._miniScript)
                ]
                //| globalVar
                //| attributeGet
                ;
        }

        rule<ScannerT> const& start() const {
            return script;
        }
    };
};

// query script

struct GetAttribute {
    void operator()(const char* nameStart_, const char* nameEnd_) const 
    { 
        std::string name (nameStart_, nameEnd_);
        if (!_script->_queryObject) {
            DomainList::const_iterator dlIt = _script->_domainList.begin ();
            for (; dlIt != _script->_domainList.end (); dlIt++) {
                if (name == (*dlIt)->getName ()) {
                    _script->_queryObject = *dlIt;
                    return;
                }
            }
        } else {
            Children::const_iterator cIt = _script->_queryObject->getChildren ().begin ();
            for (; cIt != _script->_queryObject->getChildren ().end (); cIt++) {
                if (name == (*cIt)->getName ()) {
                    _script->_queryObject = *cIt;
                    return;
                }
            }
        }
        throw std::runtime_error ("cannot find object");
    } 
    GetAttribute (MiniScript* script_) 
        : _script (script_) {} 
private: 
    MiniScript* _script;
};

struct GetElementByInteger {
    void operator()(const int& value_) const 
    { 
        if (_script->_queryObject) {
            switch (_script->_queryObject->getType()) {
                case OT_ARRAY: {
                    if (0 > value_) {
                        throw std::runtime_error ("array out of index");
                    }
                    if (_script->_queryObject->getChildren ().size () <= (size_t)value_) {
                        throw std::runtime_error ("array out of index");
                    }

                    Children const& children = _script->_queryObject->getChildren ();
                    Children::const_iterator cIt = children.begin ();
                    int i = 0;
                    while (i < value_) {
                        i++;
                        cIt++;
                    }
                    _script->_queryObject = *cIt;
                    return;
                }
                case OT_DICT: {
                    Children const& children = _script->_queryObject->getChildren ();
                    Children::const_iterator cIt = children.begin ();
                    for (; cIt != children.end (); cIt++) {
                        KeyValue* kv = dynamic_cast<KeyValue*>(*cIt);
                        Object* key = kv->getKey ();
                        if (OT_INTEGER == key->getType ()) {
                            if (value_ == dynamic_cast<Integer*>(key)->getValue ()) {
                                _script->_queryObject = kv->getValue ();
                                return;
                            }
                        }
                    }
                    throw std::runtime_error ("cannot find object");
                }
                default: {
                    throw std::runtime_error ("get element from unsupported object");
                }
            }
        }
        throw std::runtime_error ("cannot find object");
    } 
    GetElementByInteger (MiniScript* script_) 
        : _script (script_) {} 
private: 
    MiniScript* _script;
};

struct GetElementByDecimal {
    void operator()(const double& value_) const 
    { 
        if (_script->_queryObject) {
            switch (_script->_queryObject->getType()) {
                case OT_DICT: {
                    Children const& children = _script->_queryObject->getChildren ();
                    Children::const_iterator cIt = children.begin ();
                    for (; cIt != children.end (); cIt++) {
                        KeyValue* kv = dynamic_cast<KeyValue*>(*cIt);
                        Object* key = kv->getKey ();
                        if (OT_DECIMAL == key->getType ()) {
							double diff = value_ - dynamic_cast<Decimal*>(key)->getValue ();
							if ((diff >= -DBL_MIN) && (diff <= DBL_MIN)) {
                                _script->_queryObject = kv->getValue ();
                                return;
                            }
                        }
                    }
                    throw std::runtime_error ("cannot find object");
                }
                default: {
                    throw std::runtime_error ("get element from unsupported object");
                }
            }
        }
        throw std::runtime_error ("cannot find object");
    } 
    GetElementByDecimal (MiniScript* script_) 
        : _script (script_) {} 
private: 
    MiniScript* _script;
};

struct GetElementByString {
    void operator()(const char* nameStart_, const char* nameEnd_) const 
    { 
        std::string name (nameStart_ + 1, nameEnd_ - 1);
        if (_script->_queryObject) {
            switch (_script->_queryObject->getType()) {
                case OT_DICT: {
                    Children const& children = _script->_queryObject->getChildren ();
                    Children::const_iterator cIt = children.begin ();
                    for (; cIt != children.end (); cIt++) {
                        KeyValue* kv = dynamic_cast<KeyValue*>(*cIt);
                        Object* key = kv->getKey ();
                        if (OT_STRING == key->getType ()) {
                            if (name == dynamic_cast<String*>(key)->getValue ()) {
                                _script->_queryObject = kv->getValue ();
                                return;
                            }
                        }
                    }
                    throw std::runtime_error ("cannot find object");
                }
                default: {
                    throw std::runtime_error ("get element from unsupported object");
                }
            }
        }
        throw std::runtime_error ("cannot find object");
    } 
    GetElementByString (MiniScript* script_) 
        : _script (script_) {} 
private: 
    MiniScript* _script;
};

struct QueryScript : public grammar<QueryScript> {
public:
    QueryScript (MiniScript* miniScript_)
        : _miniScript (miniScript_) {}
    MiniScript* _miniScript;

    template <typename ScannerT> struct definition {

        rule<ScannerT> queryScript, attributeGet, identifier, string;
        rule<ScannerT> singleQuoteString, doubleQuoteString;

        definition(QueryScript const& self) {
            queryScript =
                attributeGet
                ;

            singleQuoteString =
                confix_p('\'', *lex_escape_ch_p, '\'')
                ;

            doubleQuoteString =
                confix_p('"', *lex_escape_ch_p, '"')
                ;

            string =
                singleQuoteString
                | doubleQuoteString;

            identifier =
                lexeme_d
                [
                    (
                        (
                            range_p('a', 'z')
                            | range_p('A', 'Z')
                            | ch_p('_')
                        )
                        >>
                        *(
                            range_p('0', '9')
                            | range_p('a', 'z')
                            | range_p('A', 'Z')
                            | ch_p('_')
                        )
                    )
                ]
                ;

            attributeGet =
                identifier
                [
                    GetAttribute (self._miniScript)
                ]
                >>
                (
                    *(
                         ch_p('.')
                         >> identifier
                            [
                                GetAttribute (self._miniScript)
                            ]
                     )
                     >>
                    *(
                         ch_p('[')
                         >>
                         (
                             longest_d
                             [
                                 strict_real_p
                                 [
                                     GetElementByDecimal (self._miniScript)
                                 ]
                                 | string
                                 [
                                     GetElementByString (self._miniScript)
                                 ]
                                 | int_p
                                 [
                                     GetElementByInteger (self._miniScript)
                                 ]
                             ]
                         )
                         >>
                         ch_p(']')
                     )
                 )
                 >>
                 !ch_p(';')
                 ;
        }

        rule<ScannerT> const& start() const {
            return queryScript;
        }
    };
};
