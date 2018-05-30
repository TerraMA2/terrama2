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
  \file terrama2/core/utility/LocalTimeUtils.cpp

  \brief Local time utils for Terrama2

  \author Jano Simas
*/

#include "LocalTimeUtils.hpp"

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
  typedef std::chrono::time_point<std::chrono::system_clock>  system_time_point;

  tm localtime(const std::time_t& time)
  {
    std::tm tm_snapshot;
  #if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
    localtime_s(&tm_snapshot, &time);
  #else
    localtime_r(&time, &tm_snapshot); // POSIX
  #endif
    return tm_snapshot;
  }

  // To simplify things the return value is just a string. I.e. by design!
  std::string put_time(const std::tm* date_time, const char* c_time_format)
  {
  #if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
    std::ostringstream oss;

    // BOGUS hack done for VS2012: C++11 non-conformant since it SHOULD take a "const struct tm*  "
    // ref. C++11 standard: ISO/IEC 14882:2011, § 27.7.1,
    oss << std::put_time(const_cast<std::tm*>(date_time), c_time_format);
    return oss.str();

  #else    // LINUX
    const size_t size = 1024;
    char buffer[size];
    auto success = std::strftime(buffer, size, c_time_format, date_time);

    if (0 == success)
      return c_time_format;

    return buffer;
  #endif
  }

  // extracting std::time_t from std:chrono for "now"
  std::time_t systemtime_now()
  {
    system_time_point system_now = std::chrono::system_clock::now();
    return std::chrono::system_clock::to_time_t(system_now);
  }

} // g2-namespace
