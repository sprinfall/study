/**
 * Copyright (c) 2006 - 2007 Etology, Inc. All Rights Reserved.
 *
 * THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF Etology, Inc.
 * The copyright notice above does not evidence any
 * actual or intended publication of such source code.
 */

/**
 * @file date_time.c
 */
#include <algorithm>

#include "date_time.h"

namespace lib
{
void date_time_t::set(const time_duration_t& timestamp_)
{
    _setHour(static_cast<int>(timestamp_.getSecondsInDay() / date_time::SECS_PER_HOUR));
    int remain = static_cast<int>(timestamp_.getSecondsInDay() % date_time::SECS_PER_HOUR);
    _setMinute(remain / date_time::SECS_PER_MIN);
    _setSecond(remain % date_time::SECS_PER_MIN);

    // Only consider time from 1901~2099, in which there's one leap year for each four years.
    long days = timestamp_.getDays() + date_time::DAYS_ADJUSTMENT; // days from 1901-01-01
    // 1461 = 4 * 365 + 1
    /*
     * - If every year contains 365 days, the tm_year can be get through: (days + 364) / 365 + 1900
     * - But indeed the 4th year contain 366 days, which will lead to tm_year increase at the last day in
     *   the four year period. (days / 1461) is the adjustment for this last day.
     */
    _setYear(static_cast<int>((days - (days / 1461) + 364) / 365 + 1900));
    int days_in_year = static_cast<int>(days - ((_year - 1901) * 365) - ((_year - 1901) / 4));

    bool is_leap_year = isLeapYear();
    const int* begin = date_time::monthlyAccumulatedDaysBegin(is_leap_year);
    const int* end = date_time::monthlyAccumulatedDaysEnd(is_leap_year);
    const int* accumulated_days = std::lower_bound(begin, end, days_in_year);

    if (accumulated_days != end)
    {
        _setMonth(static_cast<int>(accumulated_days - begin) + 1);
        if (accumulated_days == begin)
        {
            _setDay(days_in_year);
        }
        else
        {
            _setDay(days_in_year - *(accumulated_days - 1));
        }
    }
    else
    {
        // Never go here, just throw an exception
        throw std::runtime_error("Invlid days in year in date_time_t::date_time_t");
    }
}

time_duration_t date_time_t::get() const
{
    int seconds_in_day = (_hour * date_time::SECS_PER_HOUR) + (_minute * date_time::SECS_PER_MIN) + _second;
    long days = _day;
    if (1 < _month)
    {
        days += date_time::getMonthlyAccumulatedDays(isLeapYear(), _month - 1);
    }
    long years = _year - 1901;
    days += (years * 365) + (years / 4); // days since 1901-01-01 including the processed day.
    days -= date_time::DAYS_ADJUSTMENT; // days since EPOCH excluding the processd day.

    return time_duration_t(days, seconds_in_day);
}

}
