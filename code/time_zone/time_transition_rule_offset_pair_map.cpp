/**
 * Copyright (c) 2006 - 2007 Etology, Inc. All Rights Reserved.
 *
 * THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF Etology, Inc.
 * The copyright notice above does not evidence any
 * actual or intended publication of such source code.
 */

/**
 * @file global_time_transition_rule_map.c
 */
#include <string>

#include "time_transition_rule_offset_pair_map.h"

namespace lib
{
namespace detail
{
// global object to init the map
time_transition_rule_offset_pair_map_t time_transition_rule_offset_pair_map;

time_transition_rule_offset_pair_map_t::time_transition_rule_offset_pair_map_t()
{
    try {
        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t());
            _map["UTC"] = std::make_pair(rule, 0L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("Etc/GMT+12"));
            _map["ETC/GMT+12"] = std::make_pair(rule, -43200L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("Pacific/Midway"));
            _map["Pacific/Midway"] = std::make_pair(rule, -39600L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("US/Hawaii"));
            _map["US/Hawaii"] = std::make_pair(rule, -36000L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("US/Alaska"));
            _map["US/Alaska"] = std::make_pair(rule, -32400L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("US/Pacific"));
            _map["US/Pacific"] = std::make_pair(rule, -28800L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("Canada/Pacific"));
            _map["Canada/Pacific"] = std::make_pair(rule, -28800L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("US/Mountain"));
            _map["US/Mountain"] = std::make_pair(rule, -25200L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("Canada/Mountain"));
            _map["Canada/Mountain"] = std::make_pair(rule, -25200L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("US/Arizona"));
            _map["US/Arizona"] = std::make_pair(rule, -25200L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("America/Mazatlan"));
            _map["America/Mazatlan"] = std::make_pair(rule, -25200L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("Canada/Saskatchewan"));
            _map["Canada/Saskatchewan"] = std::make_pair(rule, -21600L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("US/Central"));
            _map["US/Central"] = std::make_pair(rule, -21600L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("Canada/Central"));
            _map["Canada/Central"] = std::make_pair(rule, -21600L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("America/Mexico_City"));
            _map["America/Mexico_City"] = std::make_pair(rule, -21600L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("America/Guatemala"));
            _map["America/Guatemala"] = std::make_pair(rule, -21600L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("America/Tegucigalpa"));
            _map["America/Tegucigalpa"] = std::make_pair(rule, -21600L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("America/Managua"));
            _map["America/Managua"] = std::make_pair(rule, -21600L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("America/Havana"));
            _map["America/Havana"] = std::make_pair(rule, -21600L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("America/Bogota"));
            _map["America/Bogota"] = std::make_pair(rule, -18000L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("US/Eastern"));
            _map["US/Eastern"] = std::make_pair(rule, -18000L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("Canada/Eastern"));
            _map["Canada/Eastern"] = std::make_pair(rule, -18000L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("US/East-Indiana"));
            _map["US/East-Indiana"] = std::make_pair(rule, -18000L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("Canada/Atlantic"));
            _map["Canada/Atlantic"] = std::make_pair(rule, -14400L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("America/Caracas"));
            _map["America/Caracas"] = std::make_pair(rule, -14400L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("America/Santiago"));
            _map["America/Santiago"] = std::make_pair(rule, -14400L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("Canada/Newfoundland"));
            _map["Canada/Newfoundland"] = std::make_pair(rule, -9000L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("America/Sao_Paulo"));
            _map["America/Sao_Paulo"] = std::make_pair(rule, -10800L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("America/Argentina/Buenos_Aires"));
            _map["America/Argentina/Buenos_Aires"] = std::make_pair(rule, -10800L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("America/Godthab"));
            _map["America/Godthab"] = std::make_pair(rule, -10800L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("Brazil/DeNoronha"));
            _map["Brazil/DeNoronha"] = std::make_pair(rule, -7200L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("Atlantic/South_Georgia"));
            _map["Atlantic/South_Georgia"] = std::make_pair(rule, -7200L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("Atlantic/Cape_Verde"));
            _map["Atlantic/Cape_Verde"] = std::make_pair(rule, -3600L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("Atlantic/Azores"));
            _map["Atlantic/Azores"] = std::make_pair(rule, -3600L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("Europe/London"));
            _map["Europe/London"] = std::make_pair(rule, 0L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("Africa/Casablanca"));
            _map["Africa/Casablanca"] = std::make_pair(rule, 0L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("Europe/Amsterdam"));
            _map["Europe/Amsterdam"] = std::make_pair(rule, 3600L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("Europe/Belgrade"));
            _map["Europe/Belgrade"] = std::make_pair(rule, 3600L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("Europe/Brussels"));
            _map["Europe/Brussels"] = std::make_pair(rule, 3600L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("Europe/Sarajevo"));
            _map["Europe/Sarajevo"] = std::make_pair(rule, 3600L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("Africa/Bangui"));
            _map["Africa/Bangui"] = std::make_pair(rule, 3600L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("Europe/Bucharest"));
            _map["Europe/Bucharest"] = std::make_pair(rule, 7200L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("Africa/Harare"));
            _map["Africa/Harare"] = std::make_pair(rule, 7200L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("Europe/Helsinki"));
            _map["Europe/Helsinki"] = std::make_pair(rule, 7200L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("Africa/Cairo"));
            _map["Africa/Cairo"] = std::make_pair(rule, 7200L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("Europe/Athens"));
            _map["Europe/Athens"] = std::make_pair(rule, 7200L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("Asia/Jerusalem"));
            _map["Asia/Jerusalem"] = std::make_pair(rule, 7200L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("Asia/Baghdad"));
            _map["Asia/Baghdad"] = std::make_pair(rule, 10800L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("Asia/Kuwait"));
            _map["Asia/Kuwait"] = std::make_pair(rule, 10800L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("Europe/Moscow"));
            _map["Europe/Moscow"] = std::make_pair(rule, 10800L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("Africa/Nairobi"));
            _map["Africa/Nairobi"] = std::make_pair(rule, 10800L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("Asia/Tehran"));
            _map["Asia/Tehran"] = std::make_pair(rule, 12600L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("Asia/Muscat"));
            _map["Asia/Muscat"] = std::make_pair(rule, 14400L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("Asia/Baku"));
            _map["Asia/Baku"] = std::make_pair(rule, 14400L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("Asia/Kabul"));
            _map["Asia/Kabul"] = std::make_pair(rule, 16200L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("Asia/Yekaterinburg"));
            _map["Asia/Yekaterinburg"] = std::make_pair(rule, 18000L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("Asia/Tashkent"));
            _map["Asia/Tashkent"] = std::make_pair(rule, 18000L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("Asia/Calcutta"));
            _map["Asia/Calcutta"] = std::make_pair(rule, 19800L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("Asia/Katmandu"));
            _map["Asia/Katmandu"] = std::make_pair(rule, 20700L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("Asia/Novosibirsk"));
            _map["Asia/Novosibirsk"] = std::make_pair(rule, 21600L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("Asia/Dhaka"));
            _map["Asia/Dhaka"] = std::make_pair(rule, 21600L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("Asia/Jayapura"));
            _map["Asia/Jayapura"] = std::make_pair(rule, 21600L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("Asia/Rangoon"));
            _map["Asia/Rangoon"] = std::make_pair(rule, 23400L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("Asia/Krasnoyarsk"));
            _map["Asia/Krasnoyarsk"] = std::make_pair(rule, 25200L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("Asia/Bangkok"));
            _map["Asia/Bangkok"] = std::make_pair(rule, 25200L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("Asia/Shanghai"));
            _map["Asia/Shanghai"] = std::make_pair(rule, 28800L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("Asia/Singapore"));
            _map["Asia/Singapore"] = std::make_pair(rule, 28800L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("Australia/Perth"));
            _map["Australia/Perth"] = std::make_pair(rule, 28800L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("Asia/Taipei"));
            _map["Asia/Taipei"] = std::make_pair(rule, 28800L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("Asia/Irkutsk"));
            _map["Asia/Irkutsk"] = std::make_pair(rule, 28800L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("Asia/Tokyo"));
            _map["Asia/Tokyo"] = std::make_pair(rule, 32400L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("Asia/Seoul"));
            _map["Asia/Seoul"] = std::make_pair(rule, 32400L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("Asia/Yakutsk"));
            _map["Asia/Yakutsk"] = std::make_pair(rule, 32400L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("Australia/Adelaide"));
            _map["Australia/Adelaide"] = std::make_pair(rule, 34200L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("Australia/Darwin"));
            _map["Australia/Darwin"] = std::make_pair(rule, 34200L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("Australia/Brisbane"));
            _map["Australia/Brisbane"] = std::make_pair(rule, 36000L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("Asia/Vladivostok"));
            _map["Asia/Vladivostok"] = std::make_pair(rule, 36000L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("Pacific/Guam"));
            _map["Pacific/Guam"] = std::make_pair(rule, 36000L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("Australia/Hobart"));
            _map["Australia/Hobart"] = std::make_pair(rule, 36000L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("Australia/Melbourne"));
            _map["Australia/Melbourne"] = std::make_pair(rule, 36000L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("Asia/Magadan"));
            _map["Asia/Magadan"] = std::make_pair(rule, 39600L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("Pacific/Auckland"));
            _map["Pacific/Auckland"] = std::make_pair(rule, 43200L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("Pacific/Fiji"));
            _map["Pacific/Fiji"] = std::make_pair(rule, 43200L);
        }

        {
            time_transition_rule_ptr_t rule(new time_transition_rule_t("Pacific/Tongatapu"));
            _map["Pacific/Tongatapu"] = std::make_pair(rule, 46800L);
        }
    }
    catch (...)
    {
        // clear map
        _map.clear();
    }
}

const time_transition_rule_offset_pair_map_t::mapped_type&
time_transition_rule_offset_pair_map_t::get(const key_type& name) const
{
    if (_map.empty())
    {
        throw std::runtime_error("failed in time zone initialization");
    }

    typedef std::map<key_type, mapped_type>::const_iterator pair_map_iterator_t;
    pair_map_iterator_t it = _map.find(name);
    if (it != _map.end())
    {
        return it->second;
    }

    throw std::runtime_error("unknown time zone name: " + name);
}

} // namespace detail
} // namespace lib
