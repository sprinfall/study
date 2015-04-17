/**
 * Copyright (c) 2006 - 2007 Etology, Inc. All Rights Reserved.
 *
 * THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF Etology, Inc.
 * The copyright notice above does not evidence any
 * actual or intended publication of such source code.
 */

/**
 * @file time_transition_rule.h
 * @brief Encapsulate the underlying time transition infomations for each time zone.
 */
#ifndef INCLUDED_LIB_TIME_TRANSITION_RULE_H
#define INCLUDED_LIB_TIME_TRANSITION_RULE_H

#include <string>
#include <vector>
#include <utility>

#include "config_int_types.h"
#include "time_duration.h"
#include "date_time.h"
#include "time_transition_type.h"

namespace lib
{
/**
 * @brief Transition rules for specified time zone. This class stores all the information which shall be used
 *        in process local time under the time zone.
 * @ingroup lib
 */
class time_transition_rule_t
{
public:
    class missing_zoneinfo_files {};
    class bad_transition_rule_spec {};

    time_transition_rule_t()
    {
        // empty
    }

    /**
     * @brief Read specification to get the transition rule.
     * @param rule_spec File contents stored in /usr/share/zoneinfo. Check the file format using
     *                  "man tzfile".
     * @throw missing_zoneinfo_files The zoneinfo file cannot be found.
     * @throw bad_transition_rule_spec The contents of the zoneinfo file is invalid.
     */
    time_transition_rule_t(const std::string& time_zone_name_);

    /**
     * @brief Read specification to get the transition rule.
     * @param rule_spec File contents stored in /usr/share/zoneinfo. Check the file format using
     *                  "man tzfile".
     * @throw missing_zoneinfo_files The zoneinfo file cannot be found.
     * @throw bad_transition_rule_spec The contents of the zoneinfo file is invalid.
     */
    void readRule(const std::string& time_zone_name_);

    /**
     * @brief Reset to utc rule
     */
    void clear();

    const time_transition_type_t& getDefaultTransitionType() const;

    /**
     * @brief finds out transition type in specified time point.
     * @param timestamp_ Specified time point
     * @return Trainistion type in use at the specified time point.
     */
    const time_transition_type_t& getTransitionType(const time_duration_t& timestamp_) const;

    /**
     * @brief finds out transition type in specified local time.
     * @param timestamp_ Specified local time
     * @return Trainistion type in use at the specified local time.
     */
    const time_transition_type_t& getTransitionType(const date_time_t& localtime_) const;

    /**
     * @brief finds out leap second ruls in specified time point.
     * @param time_stamp_ Specified time point
     * @return A pair, the first element is the leap seconds offset, the second identifies that this
     * second is the 61st second in that minute.
     */
    std::pair<time_offset_t, bool> getLeapSecondAdjustment(const time_duration_t& timestamp_) const;

    /**
     * @brief tells whether the specified time point is in daylight saving time.
     * @param time_stamp_ Specified time point
     * @return return true if the specified time point is in daylight saving time.
     */
    bool inDst(const time_duration_t& timestamp_) const;

    /**
     * @brief tells whether current time zone never use daylight saving time.
     * @return return true if current time zone never use daylight saving time.
     */
    bool useDst() const;
private:
    /**
     * @brief simply set default type to the last transition type which does not use
     * daylight saving time.
     */
    void _evaluateDefaultType();

    void _initTransitionLocalTimes();

    std::vector<std::pair<time_duration_t, unsigned char> > _transition_times;
    std::vector<std::pair<date_time_t, unsigned char> > _transition_local_times;
    std::vector<time_transition_type_t> _transition_types;
    std::vector<std::pair<time_duration_t, time_offset_t> > _leap_second_adjustments;
    time_transition_type_t _default_type;

    friend class time_transition_rule_test;
};

// IMPLEMENTATION
inline time_transition_rule_t::time_transition_rule_t(const std::string& time_zone_name_)
{
    readRule(time_zone_name_);
}

inline void time_transition_rule_t::clear()
{
    _transition_times.clear();
    _transition_local_times.clear();
    _transition_types.clear();
    _leap_second_adjustments.clear();
    _default_type = time_transition_type_t();
}

inline const time_transition_type_t& time_transition_rule_t::getDefaultTransitionType() const
{
    return _default_type;
}

inline bool time_transition_rule_t::useDst() const {
    return (1 < _transition_types.size());
}

// END IMPLEMENTATION

} // namespace lib

#endif // INCLUDED_LIB_TIME_TRANSITION_RULE_H
