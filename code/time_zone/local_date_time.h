/**
* Copyright (c) 2006 Etology, Inc. All Rights Reserved.
*
* THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF Etology, Inc.
* The copyright notice above does not evidence any
* actual or intended publication of such source code.
*/

/**
* @file local_date_time.h
* @brief Encapsulates local date time in system.
*/
#ifndef INCLUDED_LIB_LOCAL_DATE_TIME_H
#define INCLUDED_LIB_LOCAL_DATE_TIME_H

#include "time_duration.h"
#include "date_time.h"
#include "time_zone.h"
#include "shared_ptr.h"

namespace lib
{
    class local_date_time_t
    {
    public:
        typedef shared_ptr_t<time_zone_i> time_zone_ptr_t;

        inline local_date_time_t(time_t timestamp_, const time_zone_ptr_t& time_zone_)
            : _timestamp(time_duration_t::fromUnixTimestamp(timestamp_)), _zone(time_zone_)
        {
            _local_date_time = _zone->toLocaltime(_timestamp);
        }

        inline local_date_time_t(const time_duration_t& timestamp_, const time_zone_ptr_t& time_zone_)
            : _timestamp(timestamp_), _zone(time_zone_)
        {
            _local_date_time = _zone->toLocaltime(_timestamp);
        }

        inline local_date_time_t(const date_time_t& date_time_, const time_zone_ptr_t& time_zone_)
            : _local_date_time(date_time_), _zone(time_zone_)
        {
            _timestamp = _zone->toTimestamp(_local_date_time);
        }

        inline time_zone_ptr_t zone() const
        {
            return _zone;
        }

        inline date_time_t utcTime() const
        {
            return date_time_t(_timestamp);
        }

        inline const date_time_t& localTime() const
        {
            return _local_date_time;
        }

        inline local_date_time_t localTimeIn(const time_zone_ptr_t& time_zone_) const
        {
            return local_date_time_t(_timestamp, time_zone_);
        }

        inline const time_duration_t& toTimestamp() const
        {
            return _timestamp;
        }
    private:
        time_duration_t _timestamp;
        date_time_t _local_date_time;
        time_zone_ptr_t _zone;
    };
} // namespace lib

#endif // INCLUDED_LIB_DATE_TIME_H
