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
  \file terrama2/core/utility/CurlWrapper.hpp
  \brief Utility classes for CurlWrapper.

  \author Evandro Delatin
*/

#ifndef __TERRAMA2_CORE_UTILITY_CURLWRAPPER_HPP__
#define __TERRAMA2_CORE_UTILITY_CURLWRAPPER_HPP__

// STL
#include <memory>
#include <cassert>

// TerraMA2
#include "Raii.hpp"
#include "../../impl/DataRetrieverFTP.hpp"

// LibCurl
#include <curl/curl.h>

namespace terrama2
{
  namespace core
  {
    class CurlWrapper
    {
      public:

        CURLcode verifyURL(std::string url)
        {
          CurlPtr curl;
          curl.init();

          curl_easy_setopt(curl.fcurl(), CURLOPT_URL, url.c_str());
          curl_easy_setopt(curl.fcurl(), CURLOPT_FTPLISTONLY, 1);
          curl_easy_setopt(curl.fcurl(), CURLOPT_CONNECTTIMEOUT, 3);
          curl_easy_setopt(curl.fcurl(), CURLOPT_NOBODY, 1);

          return curl_easy_perform(curl.fcurl());
        }
    };
  }
}

#endif //__TERRAMA2_CORE_UTILITY_CURLWRAPPER_HPP__
