/**
 * Copyright (c) 2006 - 2007 Etology, Inc. All Rights Reserved.
 *
 * THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF Etology, Inc.
 * The copyright notice above does not evidence any
 * actual or intended publication of such source code.
 */

/**
 * @file time_transition_rule.c
 */
#include <fstream>
#include <iostream>

#include "shared_ptr.h"
#include "time_transition_rule.h"
#include "tzfile.h"

namespace lib
{

void time_transition_rule_t::readRule(const std::string& time_zone_name_)
{
    clear();
    std::string file_name = TZ_DIR;
    file_name.append("/");
    file_name.append(time_zone_name_);
    std::ifstream in(file_name.c_str(), std::ios::in | std::ios::binary);
    if (!in)
    {
        throw missing_zoneinfo_files();
    }

    std::istream::sentry  sentry(in, true);
    if (sentry)
    {
        union
        {
            tzfile_header_t tzheader;
            char buff[sizeof(tzfile_header_t)];
        } header;
        in.read(header.buff, sizeof(header.buff));
        if (!in && (!in.eof() || (sizeof(header.buff) == in.gcount())))
        {
            clear();
            throw bad_transition_rule_spec();
        }
        uint32_t timecnt = bin2uint32(header.tzheader.tzh_timecnt);
        uint32_t typecnt = bin2uint32(header.tzheader.tzh_typecnt);
        uint32_t charcnt = bin2uint32(header.tzheader.tzh_charcnt);
        uint32_t leapcnt = bin2uint32(header.tzheader.tzh_leapcnt);
        const size_t buff_size = get_buffer_size(timecnt, typecnt, charcnt, leapcnt);
        if (!in && (0 < buff_size))
        {
            clear();
            throw bad_transition_rule_spec();
        }
        shared_ptr_t<char> buff(new char[buff_size], stdArrayDeleter<char>);
        in.read(buff.get(), buff_size);
        if (!in && (!in.eof() || (sizeof(header.buff) == in.gcount())))
        {
            clear();
            throw bad_transition_rule_spec();
        }
        const char* times_pos = buff.get();
        const char* type_indexes_pos = times_pos + (4 * timecnt);
        // std::cout << "times-----------" << std::endl; // open for debug
        // std::cout << "Index number: " << time_zone_name_ << " " << timecnt << std::endl; // open for debug
        for (size_t i = 0; i < timecnt; ++i) {
            // std::cout << bin2int32(times_pos) << " " << int(static_cast<unsigned char>(*type_indexes_pos)) << std::endl; // open for debug
            _transition_times.push_back(
                std::make_pair(
                    time_duration_t::fromUnixTimestamp(bin2int32(times_pos)),
                    static_cast<unsigned char>(*type_indexes_pos)
                    )
            );
            times_pos += 4;
            ++type_indexes_pos;
        }
        const char* type_pos = type_indexes_pos;
        const char* char_pos = type_indexes_pos + (typecnt * 6);
        // std::cout << "\ntypes------------" << std::endl; // open for debug
        for (size_t i = 0; i < typecnt; ++i)
        {
            //std::cout << bin2int32(type_pos) << " "
            //          << (0 != static_cast<signed char>(type_pos[4])) << " "
            //          << char_pos + static_cast<unsigned char>(type_pos[5]) << std::endl; // open for debug
            _transition_types.push_back(
                time_transition_type_t(
                    bin2int32(type_pos),
                    (0 != static_cast<signed char>(type_pos[4])),
                    char_pos + static_cast<unsigned char>(type_pos[5])
                )
            );
            type_pos += 6;
        }
        // std::cout << "\nleap--------------" << std::endl; // open for debug
        const char* leap_pos = char_pos + charcnt;
        for (size_t i = 0; i < leapcnt; ++i)
        {
            // std::cout << bin2int32(leap_pos) << " " << bin2int32(leap_pos + 4); // open for debug
            _leap_second_adjustments.push_back(
                std::make_pair(
                    time_duration_t::fromUnixTimestamp(bin2int32(leap_pos)),
                    bin2int32(leap_pos + 4))
            );
            leap_pos += 8;
        }

        _evaluateDefaultType();
        _initTransitionLocalTimes();
    }
    else
    {
        throw bad_transition_rule_spec();
    }
}

void time_transition_rule_t::_initTransitionLocalTimes()
{
    time_duration_t dummy_timestamp;
    _transition_local_times.clear();

    typedef std::vector<std::pair<time_duration_t, unsigned char> >::iterator transition_iterator_t;
    for(transition_iterator_t it = _transition_times.begin();
        it != _transition_times.end();
        ++it)
    {
        dummy_timestamp = it->first + _transition_types.at(it->second).offset;
        _transition_local_times.push_back(std::make_pair(date_time_t(dummy_timestamp), it->second));
    }
}

void time_transition_rule_t::_evaluateDefaultType()
{
    if (_transition_times.empty())
    {
        // Search directly in _transition_types
        _default_type = time_transition_type_t();
        for (std::vector<time_transition_type_t>::iterator it = _transition_types.begin();
             it != _transition_types.end(); ++it)
        {
            _default_type = *it;
            if (!_default_type.is_dst)
            {
                break;
            }
        }
    }
    else
    {
        typedef std::vector<std::pair<time_duration_t, unsigned char> >::reverse_iterator transition_iterator_t;
        for(transition_iterator_t it = _transition_times.rbegin();
            it != _transition_times.rend();
            ++it)
        {
            _default_type = _transition_types.at(it->second);
            if (!_default_type.is_dst)
            {
                break;
            }
        }
    }
}

const time_transition_type_t& time_transition_rule_t::getTransitionType(const time_duration_t& timestamp_) const
{
    // binary search
    typedef std::vector<std::pair<time_duration_t, unsigned char> >::const_iterator tztype_index_iterator_t;
    tztype_index_iterator_t left  = _transition_times.begin();
    tztype_index_iterator_t right = _transition_times.end();
    tztype_index_iterator_t mid   = (right - left) / 2 + left;
    while(left != right)
    {
        if (timestamp_ < mid->first)
        {
            right = mid;
        }
        else
        {
            left = mid + 1;
            if ((left == right) || (left->first > timestamp_))
            {
                break;
            }
        }

        mid = (right - left) / 2 + left;
    }

    if (mid != _transition_times.end())
    {
        return _transition_types.at(mid->second);
    }
    else
    {
        return getDefaultTransitionType();
    }
}

const time_transition_type_t& time_transition_rule_t::getTransitionType(const date_time_t& localtime_) const
{
    // binary search
    typedef std::vector<std::pair<date_time_t, unsigned char> >::const_iterator tztype_index_iterator_t;
    tztype_index_iterator_t left  = _transition_local_times.begin();
    tztype_index_iterator_t right = _transition_local_times.end();
    tztype_index_iterator_t mid   = (right - left) / 2 + left;
    while(left != right)
    {
        if (localtime_ < mid->first)
        {
            right = mid;
        }
        else
        {
            left = mid + 1;
            if ((left == right) || (left->first > localtime_))
            {
                break;
            }
        }

        mid = (right - left) / 2 + left;
    }

    if (mid != _transition_local_times.end())
    {
        return _transition_types.at(mid->second);
    }
    else
    {
        return getDefaultTransitionType();
    }
}

std::pair<time_offset_t, bool> time_transition_rule_t::getLeapSecondAdjustment(const time_duration_t& timestamp_) const
{
    std::vector<std::pair<time_duration_t, time_offset_t> >::size_type idx = _leap_second_adjustments.size();
    std::pair<time_offset_t, bool> result = std::make_pair(static_cast<time_offset_t>(0), false);
    time_duration_t start_time;

    while (0 < idx)
    {
        --idx;

        if ((start_time = _leap_second_adjustments.at(idx).first) <= timestamp_)
        {
            result.first = _leap_second_adjustments.at(idx).second;
            if (start_time == timestamp_ && (
                    (idx == 0 && start_time > time_duration_t::ZERO)
                    || (start_time > _leap_second_adjustments.at(idx - 1).first)))
            {
                result.second = true;
            }
        }
    }

    return result;
}

bool time_transition_rule_t::inDst(const time_duration_t& timestamp_) const
{
    return getTransitionType(timestamp_).is_dst;
}

} // end of namespace lib
