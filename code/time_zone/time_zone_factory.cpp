/**
 * Copyright (c) 2006 - 2007 Etology, Inc. All Rights Reserved.
 *
 * THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF Etology, Inc.
 * The copyright notice above does not evidence any
 * actual or intended publication of such source code.
 */

/**
 * @file tzdb_time_zone_factory.cpp
 */

#include "time_zone_factory.h"
#include "tzdb_time_zone.h"
#include "time_transition_rule_offset_pair_map.h"

namespace lib
{

shared_ptr_t<time_zone_i> time_zone_factory_t::fromTzdb(const std::string& name_, bool use_dst_) throw (time_zone_init_error)
{
    try
    {
        detail::time_transition_rule_offset_pair_map_t::mapped_type rule_offset_pair
            = detail::time_transition_rule_offset_pair_map.get(name_);
        shared_ptr_t<time_zone_i> zone(
            new tzdb_time_zone_t(rule_offset_pair.first, use_dst_, rule_offset_pair.second)
        );

        return zone;
    }
    catch (const std::runtime_error& error)
    {
        throw time_zone_init_error(error.what());
    }
}

}

