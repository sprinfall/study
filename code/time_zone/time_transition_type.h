/**
 * Copyright (c) 2006 - 2007 Etology, Inc. All Rights Reserved.
 *
 * THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF Etology, Inc.
 * The copyright notice above does not evidence any
 * actual or intended publication of such source code.
 */

/**
 * @file time_transition_type.h
 */
#ifndef INCLUDED_LIB_TIME_TRANSITION_TYPE_H
#define INCLUDED_LIB_TIME_TRANSITION_TYPE_H

#include <string>

#include "config_int_types.h"

namespace lib
{
typedef int32_t time_offset_t;
/**
 * @brief Specifies the offset to UTC.
 * @ingroup lib
 */
class time_transition_type_t
{
public:
    time_offset_t offset;       /**< Specifies the offset to UTC. **/
    bool         is_dst;       /**< Tells whether this type is for daylight savings. **/
    std::string   name;         /**< Time zone name. **/

    time_transition_type_t()
        : offset(0), is_dst(false), name("UTC")
    {
        // empty
    }

    time_transition_type_t(int32_t offset_, bool is_dst_, const std::string& name_)
        : offset(offset_), is_dst(is_dst_), name(name_)
    {
        // empty
    }
};

} // namespace lib

#endif // INCLUDED_LIB_TIME_TRANSITION_TYPE_H
