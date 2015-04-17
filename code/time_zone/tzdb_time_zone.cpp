/**
 * Copyright (c) 2006 - 2007 Etology, Inc. All Rights Reserved.
 *
 * THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF Etology, Inc.
 * The copyright notice above does not evidence any
 * actual or intended publication of such source code.
 */

/**
 * @file tzdb_time_zone.c
 */
#include "tzdb_time_zone.h"

namespace lib
{
tzdb_time_zone_t::tzdb_time_zone_t(time_transition_rule_ptr_t rule_, bool use_dst_)
    : _rule(rule_), _use_dst(use_dst_)
{
    _default_type = _rule->getDefaultTransitionType();
}

tzdb_time_zone_t::tzdb_time_zone_t(time_transition_rule_ptr_t rule_, bool use_dst_, int32_t default_offset_)
    : _rule(rule_), _use_dst(use_dst_)
{
    _default_type = _rule->getDefaultTransitionType();
    _default_type.offset = default_offset_;
}

date_time_t tzdb_time_zone_t::toLocaltime(const time_duration_t& timestamp_) const
{
    time_offset_t offset = _use_dst ? _rule->getTransitionType(timestamp_).offset : _default_type.offset;
    return date_time_t(timestamp_ + offset);
}

time_duration_t tzdb_time_zone_t::toTimestamp(const date_time_t& date_time_) const
{
    time_offset_t offset = _use_dst ? _rule->getTransitionType(date_time_).offset : _default_type.offset;
    return date_time_.get() - offset;
}

}

