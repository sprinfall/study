/**
 * Copyright (c) 2006 - 2007 Etology, Inc. All Rights Reserved.
 *
 * THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF Etology, Inc.
 * The copyright notice above does not evidence any
 * actual or intended publication of such source code.
 */

/**
 * @file time_duration.h
 */
#ifndef INCLUDED_LIB_TIME_DURATION_H
#define INCLUDED_LIB_TIME_DURATION_H
#include <string>
#include <time.h>

namespace lib
{
/**
 * @brief A length of time unattached to any point on the time continuum.
 * @ingroup lib
 */
class time_duration_t
{
public:
    static time_duration_t fromUnixTimestamp(time_t stamp_);
    static const time_duration_t ZERO;
    explicit time_duration_t(long days_ = 0, long seconds_in_day_ = 0);
    void setSecondsInDay(long seconds_in_day_);
    void setDays(long days_);
    long getDays() const;
    long getSecondsInDay() const;
    bool lessThan(const time_duration_t& rhs) const;
    bool equal(const time_duration_t& rhs) const;
    time_duration_t& operator +=(const time_duration_t& rhs);
    time_duration_t& operator +=(long seconds);
    time_duration_t& operator -=(const time_duration_t& rhs);
    time_duration_t& operator -=(long seconds);
    time_duration_t& operator /=(int dividor);

    /**
     * @brief return unix timestamp
     * @return unix timestamp
     * @warning Returned timestamp may overflow!
     */
    time_t unixTimestamp() const;
private:
    enum
    {
        SECONDS_IN_DAY = 86400
    };

    long _days;
    long _seconds_in_day;
}; // end class time_duration_t

inline time_duration_t operator +(const time_duration_t& a, const time_duration_t& b) {
    time_duration_t result(a);
    result += b;

    return result;
}

inline time_duration_t operator +(const time_duration_t& a, long seconds) {
    time_duration_t result(a);
    result += seconds;

    return result;
}

inline time_duration_t operator +(long seconds, const time_duration_t& a)
{
    return a + seconds;
}

inline time_duration_t operator -(const time_duration_t& a, const time_duration_t& b) {
    time_duration_t result(a);
    result -= b;

    return result;
}

inline time_duration_t operator -(const time_duration_t& a, long seconds) {
    time_duration_t result(a);
    result -= seconds;

    return result;
}

inline time_duration_t operator /(const time_duration_t& a, int divisor)
{
    time_duration_t result(a);
    result /= divisor;

    return result;
}

inline bool operator ==(const time_duration_t& a, const time_duration_t& b)
{
    return a.equal(b);
}

inline bool operator !=(const time_duration_t& a, const time_duration_t& b)
{
    return !(a.equal(b));
}

inline bool operator <(const time_duration_t& a, const time_duration_t& b) {
    return a.lessThan(b);
}

inline bool operator >(const time_duration_t& a, const time_duration_t& b) {
    return !(a.lessThan(b)) && !(a.equal(b));
}

inline bool operator <=(const time_duration_t& a, const time_duration_t& b) {
    return a.lessThan(b) || a.equal(b);
}

inline bool operator >=(const time_duration_t& a, const time_duration_t& b) {
    return !(a.lessThan(b));
}

// IMPLEMENTATION

inline time_duration_t time_duration_t::fromUnixTimestamp(time_t stamp_)
{
    long days = stamp_ / time_duration_t::SECONDS_IN_DAY;
    long seconds_in_day = stamp_ - days * time_duration_t::SECONDS_IN_DAY;
    if (0 > seconds_in_day) // prevent that some platform generates negative remainder
    {
        --days;
        seconds_in_day += time_duration_t::SECONDS_IN_DAY;
    }

    return time_duration_t(days, seconds_in_day);
}

inline time_duration_t::time_duration_t(long days_, long seconds_in_day_)
{
    setDays(days_);
    setSecondsInDay(seconds_in_day_);
}

inline void time_duration_t::setSecondsInDay(long seconds_in_day_)
{
    _seconds_in_day = seconds_in_day_;
    while (0 > _seconds_in_day)
    {
        --_days;
        _seconds_in_day += time_duration_t::SECONDS_IN_DAY;
    }
    while (_seconds_in_day >= time_duration_t::SECONDS_IN_DAY)
    {
        ++_days;
        _seconds_in_day -= time_duration_t::SECONDS_IN_DAY;
    }
}

inline void time_duration_t::setDays(long days_)
{
    _days = days_;
}

inline long time_duration_t::getDays() const
{
    return _days;
}

inline long time_duration_t::getSecondsInDay() const
{
    return _seconds_in_day;
}

inline time_duration_t& time_duration_t::operator +=(const time_duration_t& rhs)
{
    setDays(getDays() + rhs.getDays());
    setSecondsInDay(getSecondsInDay() + rhs.getSecondsInDay());
}

inline time_duration_t& time_duration_t::operator +=(long seconds)
{
    setSecondsInDay(getSecondsInDay() + seconds);
}

inline time_duration_t& time_duration_t::operator -=(const time_duration_t& rhs)
{
    setDays(getDays() - rhs.getDays());
    setSecondsInDay(getSecondsInDay() - rhs.getSecondsInDay());
}

inline time_duration_t& time_duration_t::operator -=(long seconds)
{
    setSecondsInDay(getSecondsInDay() - seconds);
}

inline time_duration_t& time_duration_t::operator /=(int divisor)
{
    setDays(getDays() / divisor);
    setSecondsInDay(getSecondsInDay() / divisor);
}

inline bool time_duration_t::lessThan(const time_duration_t& rhs) const
{
    bool less = false;
    if (getDays() < rhs.getDays())
    {
        less = true;
    }
    else if (getDays() == rhs.getDays()) {
        less = (getSecondsInDay() < rhs.getSecondsInDay());
    }

    return less;
}

inline bool time_duration_t::equal(const time_duration_t& rhs) const
{
    return (getDays() == rhs.getDays())
           && (getSecondsInDay() == rhs.getSecondsInDay());
}

inline time_t time_duration_t::unixTimestamp() const
{
    return getDays() * time_duration_t::SECONDS_IN_DAY + getSecondsInDay();
}

// END IMPLEMENTATION

} // end namespace lib

#endif // INCLUDED_LIB_TIME_DURATION_H

