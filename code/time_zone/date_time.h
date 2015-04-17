/**
 * Copyright (c) 2006 Etology, Inc. All Rights Reserved.
 *
 * THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF Etology, Inc.
 * The copyright notice above does not evidence any
 * actual or intended publication of such source code.
 */

/**
 * @file date_time.h
 * @brief Encapsulates date time in system.
 */
#ifndef INCLUDED_LIB_DATE_TIME_H
#define INCLUDED_LIB_DATE_TIME_H

#include <stdexcept>
#include <string>
#include <sstream>
#include <iomanip>

#include "date_time_constants.h"
#include "time_duration.h"

namespace lib
{

class date_time_t
{
public:
    // Creator
    date_time_t();
    explicit date_time_t(time_t timestamp_);
    explicit date_time_t(const time_duration_t& timestamp_);
    explicit date_time_t(int year_, int month_, int day_,
            int hour_ = 0, int minute_ = 0, int second_ = 0) throw (std::out_of_range);

    // Manipulator
    void set(const time_duration_t& timestamp_);

    // Accessor
    time_duration_t get() const;

    std::string str() const;
    std::string dateStr() const;
    std::string timeStr() const;

    date_time_t date() const;
    unsigned long dateNumber() const;
    unsigned long timeNumber() const;

    bool isLeapYear() const;
    int getYear() const;
    int getMonth() const;
    int getDay() const;
    int getHour() const;
    int getMinute() const;
    int getSecond() const;

    bool equal(const date_time_t& rhs_) const;
    bool lessThan(const date_time_t& rhs_) const;

private:
    inline void _setYear(int year_) throw (std::out_of_range);
    inline void _setMonth(int month_) throw (std::out_of_range);
    inline void _setDay(int day_) throw (std::out_of_range);
    inline void _setHour(int hour_) throw (std::out_of_range);
    inline void _setMinute(int minute_) throw (std::out_of_range);
    inline void _setSecond(int second_) throw (std::out_of_range);

    int _year;
    int _month;
    int _day;
    int _hour;
    int _minute;
    int _second;
};

inline bool operator <(const date_time_t& a, const date_time_t& b)
{
    return a.lessThan(b);
}

inline bool operator ==(const date_time_t& a, const date_time_t& b)
{
    return a.equal(b);
}

inline bool operator <=(const date_time_t& a, const date_time_t& b)
{
    return a.lessThan(b) || a.equal(b);
}

inline bool operator !=(const date_time_t& a, const date_time_t& b)
{
    return !(a.equal(b));
}

inline bool operator >=(const date_time_t& a, const date_time_t& b)
{
    return !(a.lessThan(b));
}

inline bool operator >(const date_time_t& a, const date_time_t& b)
{
    return !(a.equal(b)) && !(a.lessThan(b));
}

// IMPLEMENTATION

inline date_time_t::date_time_t()
    : _year(1970), _month(1), _day(1), _hour(0), _minute(0), _second(0)
{
    // empty
}

inline date_time_t::date_time_t(const time_duration_t& timestamp_)
    : _month(1), _day(1)
{
    set(timestamp_);
}

inline date_time_t::date_time_t(time_t timestamp_)
    : _month(1), _day(1)
{
    set(time_duration_t::fromUnixTimestamp(timestamp_));
}

inline date_time_t::date_time_t(int year_, int month_, int day_,
        int hour_, int minute_, int second_) throw (std::out_of_range)
    : _month(1), _day(1)
{
    _setSecond(second_);
    _setMinute(minute_);
    _setHour(hour_);
    _setYear(year_);
    _setMonth(month_);
    _setDay(day_);
}

inline std::string date_time_t::str() const
{
    std::ostringstream os;

    os.fill('0');

    os << std::setw(4) << _year << '-'
       << std::setw(2) << _month << '-'
       << std::setw(2) << _day << ' '
       << std::setw(2) << _hour << ':'
       << std::setw(2) << _minute << ':'
       << std::setw(2) << _second;


    return os.str();
}

inline std::string date_time_t::dateStr() const
{
    std::ostringstream os;

    os.fill('0');

    os << std::setw(4) << _year << '-'
       << std::setw(2) << _month << '-'
       << std::setw(2) << _day;

    return os.str();
}

inline std::string date_time_t::timeStr() const
{
    std::ostringstream os;

    os.fill('0');

    os << std::setw(2) << _hour << ':'
       << std::setw(2) << _minute << ':'
       << std::setw(2) << _second;

    return os.str();
}

inline date_time_t date_time_t::date() const
{
    return date_time_t(_year, _month, _day);
}

inline unsigned long date_time_t::dateNumber() const
{
    return _year * 10000UL + _month * 100UL + _day;
}

inline unsigned long date_time_t::timeNumber() const
{
    return _hour * 10000UL + _minute * 100UL + _second;
}

inline bool date_time_t::isLeapYear() const
{
    // 1. Every year divisible by 4 is a leap year.
    // 2. But every year divisible by 100 is NOT a leap year.
    // 3. Unless the year is also divisible by 400, then it is still a leap year.
    return ((0 == _year % 4) && (0 != _year % 100)) || (0 == _year % 400);
}

inline bool date_time_t::equal(const date_time_t& rhs_) const
{
    return (rhs_._minute == _minute)
        && (rhs_._second == _second)
        && (rhs_._hour == _hour)
        && (rhs_._day == _day)
        && (rhs_._month == _month)
        && (rhs_._year == _year);
}

inline bool date_time_t::lessThan(const date_time_t& rhs_) const
{
    if (_year != rhs_._year)
    {
        return _year < rhs_._year;
    }

    if (_month != rhs_._month)
    {
        return _month < rhs_._month;
    }

    if (_day != rhs_._day)
    {
        return _day < rhs_._day;
    }

    if (_hour != rhs_._hour)
    {
        return _hour < rhs_._hour;
    }

    if (_minute != rhs_._minute)
    {
        return _minute < rhs_._minute;
    }

    return _second < rhs_._second;
}

inline int date_time_t::getYear() const
{
    return _year;
}

inline int date_time_t::getMonth() const
{
    return _month;
}

inline int date_time_t::getDay() const
{
    return _day;
}

inline int date_time_t::getHour() const
{
    return _hour;
}

inline int date_time_t::getMinute() const
{
    return _minute;
}

inline int date_time_t::getSecond() const
{
    return _second;
}

inline void date_time_t::_setYear(int year_) throw (std::out_of_range)
{
    _year = year_;
    _setDay(_day);
}

inline void date_time_t::_setMonth(int month_) throw (std::out_of_range)
{
    if (1 <= month_ && month_ <= 12)
    {
        _month = month_;
        _setDay(_day);
    }
    else
    {
        throw std::out_of_range("date_time_t::setMonth month out of range");
    }
}

inline void date_time_t::_setDay(int day_) throw (std::out_of_range)
{
    int max_days = date_time::getMonthDays(isLeapYear(), _month);

    if (1 <= day_ && day_ <= max_days)
    {
        _day = day_;
    }
    else
    {
        throw std::out_of_range("date_time_t::setDay day in month out of range");
    }
}

inline void date_time_t::_setHour(int hour_) throw (std::out_of_range)
{
    if (0 <= hour_ && hour_ < 24)
    {
        _hour = hour_;
    }
    else
    {
        throw std::out_of_range("date_time_t::setHour hour out of range");
    }
}

inline void date_time_t::_setMinute(int minute_) throw (std::out_of_range)
{
    if (0 <= minute_ && minute_ < 60)
    {
        _minute = minute_;
    }
    else
    {
        throw std::out_of_range("date_time_t::setMinute minute out of range");
    }
}

inline void date_time_t::_setSecond(int second_) throw (std::out_of_range)
{
    if (0 <= second_ && second_ <= 60)
    {
        _second = second_;
    }
    else
    {
        throw std::out_of_range("date_time_t::setSecond second out of range");
    }
}

// END IMPLEMENTATION

} // namespace lib

#endif // INCLUDED_LIB_DATE_TIME_H
