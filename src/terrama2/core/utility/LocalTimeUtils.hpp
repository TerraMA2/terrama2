/*
  Copyright (C) 2007 National Institute For Space Research (INPE) - Brazil.

  This file is part of TerraMA2 - a free and open source computational
  platform for analysis, monitoring, and alert of geo-environmental extremes.

  TerraMA2 is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation, either version 3 of the License,
  or (at your option) any later version.

  TerraMA2 is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with TerraMA2. See LICENSE. If not, write to
  TerraMA2 Team at <terrama2-team@dpi.inpe.br>.
*/

/*!
  \file terrama2/core/utility/LocalTimeUtils.hpp

  \brief Local time utils for Terrama2

  \author Jano Simas
*/

#ifndef __TERRAMA2_CORE_LOCAL_TIME_UTILS_HPP__
#define __TERRAMA2_CORE_LOCAL_TIME_UTILS_HPP__

#include <chrono>
#include <string>
//////////////////////////////////////////////////////////
//
// For the current versions of GCC and clang the put_time function is not implemented
// and only partially implemented in MSVC
//
// this code implements the necessary functions to find the current system timezone
// it is originaly from: https://kjellkod.wordpress.com/2013/01/22/exploring-c11-part-2-localtime-and-time-again/
//
// Can be remove when the STL time functions are implemented.
//
// note: This functions are thread safe
//
namespace g2
{
  tm localtime(const std::time_t& time);
  std::string put_time(const std::tm* date_time, const char* c_time_format);
  std::time_t systemtime_now();
}

#endif //__TERRAMA2_CORE_LOCAL_TIME_UTILS_HPP__
