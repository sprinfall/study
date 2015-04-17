/**
 * Copyright (c) 2006 - 2007 Etology, Inc. All Rights Reserved.
 *
 * THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF Etology, Inc.
 * The copyright notice above does not evidence any
 * actual or intended publication of such source code.
 */

/**
 * @file date_time_constants.h
 * @brief Constants need to be used in date time processing functions.
 */
#ifndef INCLUDED_LIB_DATE_TIME_CONSTANTS_H
#define INCLUDED_LIB_DATE_TIME_CONSTANTS_H

namespace lib {
/**
 * @brief Constants need to be used in date time processing functions.
 * @ingroup lib
 */
namespace date_time {
/**
 * @defgroup lib_date_time_constants Date Time Constants
 * @ingroup lib
 * @{
 */
const int SECS_PER_MIN  = 60;
const int MINS_PER_HOUR = 60;
const int HOURS_PER_DAY = 24;
const int DAYS_PER_WEEK = 7;
const int SECS_PER_HOUR = SECS_PER_MIN * MINS_PER_HOUR;
const int SECS_PER_DAY  = SECS_PER_HOUR * HOURS_PER_DAY;

const int SUNDAY    = 0;
const int MONDAY    = 1;
const int TUESDAY   = 2;
const int WEDNESDAY = 3;
const int THURSDAY  = 4;
const int FRIDAY    = 5;
const int SATURDAY  = 6;

const int EPOCH_WDAY = THURSDAY;
const int EPOCH_YEAR = 1970;
const int DAYS_ADJUSTMENT = 25203;  // days in [1901-01-01, 1970-01-01]

namespace { // anonymous namespace
const int MONTHLY_ACCUMULATED_DAYS[2][12] = {
    //MON  1   2     3   4    5    6    7    8    9    10   11   12
    //DAYS 31  28/29 31  30   31   30   31   31   30   31   30   31
    {  31, 59,   90, 120, 151, 181, 212, 243, 273, 304, 334, 365 },
    {  31, 60,   91, 121, 152, 182, 213, 244, 274, 305, 335, 366 }
};
}  // end anonymous namespace

inline const int* monthlyAccumulatedDaysBegin(bool is_leap_year_)
{
    return MONTHLY_ACCUMULATED_DAYS[is_leap_year_ ? 1 : 0];
}

inline const int* monthlyAccumulatedDaysEnd(bool is_leap_year_)
{
    return MONTHLY_ACCUMULATED_DAYS[is_leap_year_ ? 1 : 0] + 12;
}

inline int getMonthlyAccumulatedDays(bool is_leap_year_, int month_)
{
    return MONTHLY_ACCUMULATED_DAYS[is_leap_year_ ? 1 : 0][(month_ - 1) % 12];
}

inline int getMonthDays(bool is_leap_year_, int month_)
{
    const int* accumulated_days = MONTHLY_ACCUMULATED_DAYS[is_leap_year_ ? 1 : 0];
    month_ = (month_ - 1) % 12;
    if (0 == month_)
    {
        return accumulated_days[0];
    }
    else
    {
        return accumulated_days[month_] - accumulated_days[month_ - 1];
    }
}

/** @} end group lib_date_time_constants */
} // namespace date_time

} // namespace lib

#endif // INCLUDED_LIB_DATE_TIME_CONSTANTS_H
