/**
 * Copyright (c) 2006 Etology, Inc. All Rights Reserved.
 *
 * THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF Etology, Inc.
 * The copyright notice above does not evidence any
 * actual or intended publication of such source code.
 */

/**
 * @file tzfile.hh
 * @brief Description about the format of the zoneinfo database.
 */
#ifndef INCLUDED_LIB_TZFILE_HH
#define INCLUDED_LIB_TZFILE_HH

#include "config_int_types.h"

namespace lib
{
/**
 * @brief Header
 */
struct tzfile_header_t
{
    char tzh_magic[4];      /**< TZ_MAGIC */
    char tzh_reserved[16];  /**< reserved for future use. */
    char tzh_ttisgmtcnt[4]; /**< The number of UTC/local indicators stored in the file. */
    char tzh_ttisstdcnt[4]; /**< The number of standard/wall indicators stored in the file. */
    char tzh_leapcnt[4];    /**< The number of leap seconds for which data is stored in the file. */
    char tzh_timecnt[4];    /**< The number of "transition times" for which data is stored in the file. */
    char tzh_typecnt[4];    /**< The number of "local time types" for which data is stored in the file
                                 (must not be zero). */
    char tzh_charcnt[4];    /**< The number of characters of "time zone abbreviation strings" stored in
                                 the file. */
};

/*
 * The above header is followed by:
 * - tzh_timecnt four-byte values of type long, sorted in ascending order. These values are written in
 *   "standard byte order". Each is used as a transition time at which the rules for computing local time
 *   change.
 * - tzh_timecnt one-byte values of type unsigned char, each one tells which of the different types of
 *   "loocal time" types described in the file is associated with the same-indexed transition time. These
 *   values serve as indices into an array of ttinfo structures that appears next in the file. These
 *   structures are defined as follows:
 *      struct ttinfo
 *      {
 *          char tt_gmtoff[4];          // The number of seconds to be added to UTC.
 *          unsigned char tt_isdst;     // Tells whether in daylight saving time.
 *          unsigned char tt_abbrind;   // Index into the array of time zone abbreviation characters.
 *      };
 * - tzh_typecnt ttinfo's.
 * - tzh_charcnt abbreviation characters.
 * - tzh_leapcnt pair of four-byte values. The first value of each pair gives the time at which a leap second
 *   occurs; the second gives the total number of leap seconds to be applied after the given time. The pairs
 *   of values are sorted in ascending order by time.
 * - tzh_ttisstdcnt (char)s indexed by type; if TRUE, transition time is standard time, if FALSE, transition
 *   time is wall clock time, if absent, transition times are assumed to be wall clock time.
 * - tzh_ttisgmtcnt (char)s indexed by type; if TRUE, transition time is UTC, if FALSE, transition time is
 *   local time, if absent, transition times are assumed to be local time
 */

inline size_t get_buffer_size(size_t tzh_timecnt, size_t tzh_typecnt,
                              size_t tzh_charcnt, size_t tzh_leapcnt)
{
    return (tzh_timecnt * 5) +
            (tzh_typecnt * 6) +
            tzh_charcnt +
            (tzh_leapcnt * 8);
}

inline uint32_t bin2uint32(const char* bytes)
{
    return static_cast<uint32_t>(static_cast<unsigned char>(bytes[3])) |
        (static_cast<uint32_t>(static_cast<unsigned char>(bytes[2])) << 8) |
        (static_cast<uint32_t>(static_cast<unsigned char>(bytes[1])) << 16) |
        (static_cast<uint32_t>(static_cast<unsigned char>(bytes[0])) << 24);
}

inline int32_t bin2int32(const char* bytes)
{
    return static_cast<int32_t>(bin2uint32(bytes));
}

inline uint8_t bin2uint8(const char* byte)
{
    return static_cast<unsigned char>(*byte);
}

inline int8_t bin2int8(const char* byte)
{
    return static_cast<signed char>(*byte);
}

namespace {
/**
 * @biref Directory storing zoneinfo database
 */
const char* TZ_DIR = "/usr/share/zoneinfo";

/**
 * @brief Magic string specifying that the file stores time zone info.
 */
const char* TZ_MAGIC = "TZif";

/**
 * @brief Time zone in use in current system.
 * @warning DO synchronize with the time zone list in ecommerce.
 */
const char* TIME_ZONES[] =
{
   "ETC/GMT+12",
   "Pacific/Midway",
   "US/Hawaii",
   "US/Alaska",
   "US/Pacific",
   "Canada/Pacific",
   "US/Mountain",
   "Canada/Mountain",
   "US/Arizona",
   "America/Mazatlan",
   "Canada/Saskatchewan",
   "US/Central",
   "Canada/Central",
   "America/Mexico_City",
   "America/Guatemala",
   "America/Tegucigalpa",
   "America/Managua",
   "America/Havana",
   "America/Bogota",
   "US/Eastern",
   "Canada/Eastern",
   "US/East-Indiana",
   "Canada/Atlantic",
   "America/Caracas",
   "America/Santiago",
   "Canada/Newfoundland",
   "America/Sao_Paulo",
   "America/Argentina/Buenos_Aires",
   "America/Godthab",
   "Brazil/DeNoronha",
   "Atlantic/South_Georgia",
   "Atlantic/Cape_Verde",
   "Atlantic/Azores",
   "Europe/London",
   "Africa/Casablanca",
   "Europe/Amsterdam",
   "Europe/Belgrade",
   "Europe/Brussels",
   "Europe/Sarajevo",
   "Africa/Bangui",
   "Europe/Bucharest",
   "Africa/Harare",
   "Europe/Helsinki",
   "Africa/Cairo",
   "Europe/Athens",
   "Asia/Jerusalem",
   "Asia/Baghdad",
   "Asia/Kuwait",
   "Europe/Moscow",
   "Africa/Nairobi",
   "Asia/Tehran",
   "Asia/Muscat",
   "Asia/Baku",
   "Asia/Kabul",
   "Asia/Yekaterinburg",
   "Asia/Tashkent",
   "Asia/Calcutta",
   "Asia/Katmandu",
   "Asia/Novosibirsk",
   "Asia/Dhaka",
   "Asia/Jayapura",
   "Asia/Rangoon",
   "Asia/Krasnoyarsk",
   "Asia/Bangkok",
   "Asia/Shanghai",
   "Asia/Singapore",
   "Australia/Perth",
   "Asia/Taipei",
   "Asia/Irkutsk",
   "Asia/Tokyo",
   "Asia/Seoul",
   "Asia/Yakutsk",
   "Australia/Adelaide",
   "Australia/Darwin",
   "Australia/Brisbane",
   "Asia/Vladivostok",
   "Pacific/Guam",
   "Australia/Hobart",
   "Australia/Melbourne",
   "Asia/Magadan",
   "Pacific/Auckland",
   "Pacific/Fiji",
   "Pacific/Tongatapu"
};

} // end anonymous namespace
} // namespace lib

#endif // INCLUDED_LIB_TZFILE_HH

