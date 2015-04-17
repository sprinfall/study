/**
 * Copyright (c) 2006 - 2007 Etology, Inc. All Rights Reserved.
 *
 * THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF Etology, Inc.
 * The copyright notice above does not evidence any
 * actual or intended publication of such source code.
 */

/**
 * @file time_zone.hh
 * @brief Time zone interface.
 */
#ifndef INCLUDED_LIB_TIME_ZONE_H
#define INCLUDED_LIB_TIME_ZONE_H

#include <stdexcept>

#include "time_duration.h"
#include "date_time.h"

namespace lib {

class time_zone_i
{
public:
    virtual ~time_zone_i()
    {
        // empty
    }

    virtual date_time_t toLocaltime(const time_duration_t& timestamp_) const = 0;

    date_time_t toLocaltime(time_t timestamp_) const
    {
        return toLocaltime(time_duration_t::fromUnixTimestamp(timestamp_));
    }

    virtual time_duration_t toTimestamp(const date_time_t& date_time_) const = 0;
};

class time_zone_init_error : public std::runtime_error
{
public:
    time_zone_init_error(const std::string& error)
        : std::runtime_error(error)
    {
        // empty
    }
};

} // namespace lib

#endif // INCLUDED_LIB_TIME_ZONE_H

