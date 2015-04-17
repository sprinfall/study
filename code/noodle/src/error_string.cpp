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

#include "noodle.hpp"
#include "error_string.hpp"

const char* ErrorString::errorString (int error_) {
    switch (error_) {
        case OK:
            return "OK";
        case FAIL:
            return "internal error";
        case ERR_BAD_ALLOC:
            return "bad alloc";
        case ERR_UNDEFINED_STATE:
            return "undefined subject state";
        case ERR_NEED_ATTACH:
            return "reserved error";
        case ERR_SAME_SUBJECT:
            return "reserved error";
        case ERR_UNDEFINED_INSTANCE:
            return "undefined instance";
        case ERR_UNKNOWN_MODULE:
            return "unknown plugin";
        case ERR_LOAD_MODULE:
            return "load plugin failed";
        case ERR_UNDEFINED_MODULE_PROC:
            return "need define a entry procedure for plugin";
        case ERR_DEAD_SUBJECT:
            return "the instance has been already destroyed";
        case ERR_DUP_MODULE:
            return "try to load loaded plugin";
        case ERR_DUP_INSTANCE:
            return "the instance is existed";
        case ERR_UNKNOWN_INSTANCE:
            return "unknown instance";
        case ERR_UNKNOWN_PROXY:
            return "unknown proxy";
        case ERR_DUP_PROXY:
            return "the proxy has been registered";
        case ERR_UNIMPL:
            return "unimplemeted";
        case ERR_DISABLED_SUBJECT:
            return "the instance has been disabled";
        default:
            break;
    }

    return "unknown error";
}
