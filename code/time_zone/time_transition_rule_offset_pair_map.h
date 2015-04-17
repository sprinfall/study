/**
 * Copyright (c) 2006 - 2007 Etology, Inc. All Rights Reserved.
 *
 * THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF Etology, Inc.
 * The copyright notice above does not evidence any
 * actual or intended publication of such source code.
 */

#ifndef INCLUDED_TIME_TRANSITION_RULE_OFFSET_PAIR_MAP_H
#define INCLUDED_TIME_TRANSITION_RULE_OFFSET_PAIR_MAP_H
/**
 * @file time_transition_rule_offset_pair_map.h
 * @brief Init the map in static section.
 */
#include <string>
#include <map>
#include <utility>
#include <stdexcept>

#include "time_transition_rule.h"
#include "shared_ptr.h"
#include "time_zone.h"

namespace lib {

namespace detail {
class time_transition_rule_offset_pair_map_t
{
public:
    typedef shared_ptr_t<time_transition_rule_t> time_transition_rule_ptr_t;
    typedef std::pair<time_transition_rule_ptr_t, long> time_transition_rule_offset_pair_t;
    typedef time_transition_rule_offset_pair_t mapped_type;
    typedef std::string                        key_type;

    time_transition_rule_offset_pair_map_t();

    const mapped_type& get(const key_type& name) const;

private:
    std::map<key_type, mapped_type> _map;
};

extern time_transition_rule_offset_pair_map_t time_transition_rule_offset_pair_map;

} // namespace detail

} // namespace lib

#endif // INCLUDED_TIME_TRANSITION_RULE_OFFSET_PAIR_MAP_H

