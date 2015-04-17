/**
 * Copyright (c) 2006 - 2007 Etology, Inc. All Rights Reserved.
 *
 * THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF Etology, Inc.
 * The copyright notice above does not evidence any
 * actual or intended publication of such source code.
 */

/**
 * @file time_zone_factory.h
 * @brief Encapsulate the tzdb_time_zone_t implementation.
 */
#ifndef INCLUDED_TIME_ZONE_FACTORY_H
#define INCLUDED_TIME_ZONE_FACTORY_H

#include <string>
#include <map>

#include "time_zone.h"
#include "shared_ptr.h"

namespace lib
{

class time_zone_factory_t
{
public:
    static shared_ptr_t<time_zone_i> fromTzdb(const std::string& name_, bool use_dst_) throw (time_zone_init_error);
};

}

#endif // INCLUDED_TIME_ZONE_FACTORY_H
