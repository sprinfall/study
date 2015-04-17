/**
 * Copyright (c) 2006 - 2007 Etology, Inc. All Rights Reserved.
 *
 * THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF Etology, Inc.
 * The copyright notice above does not evidence any
 * actual or intended publication of such source code.
 */

/**
 * @file tzdb_time_zone.hh
 * @brief Time zone implementation using zone info in time zone database.
 */
#ifndef INCLUDED_TZDB_TIME_ZONE_H
#define INCLUDED_TZDB_TIME_ZONE_H

#include <string>

#include "config_int_types.h"
#include "shared_ptr.h"
#include "time_zone.h"
#include "time_transition_type.h"
#include "time_transition_rule.h"

namespace lib
{

class tzdb_time_zone_t : public time_zone_i
{
public:
    typedef shared_ptr_t<time_transition_rule_t> time_transition_rule_ptr_t;

    tzdb_time_zone_t(time_transition_rule_ptr_t rule_, bool use_dst_);

    tzdb_time_zone_t(time_transition_rule_ptr_t rule_, bool use_dst_, int32_t default_offset_);

    date_time_t toLocaltime(const time_duration_t& timestamp_) const;

    time_duration_t toTimestamp(const date_time_t& date_time_) const;

private:
    time_transition_rule_ptr_t _rule;
    bool _use_dst;
    time_transition_type_t _default_type;
};

}

#endif // INCLUDED_TZDB_TIME_ZONE_H

