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
  \file terrama2/core/utility/CurlPtr.hpp
  \brief Utility classes for CurlPtr.

  \author Evandro Delatin
*/

#ifndef __TERRAMA2_CORE_UTILITY_CURLPTR_HPP__
#define __TERRAMA2_CORE_UTILITY_CURLPTR_HPP__

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
    //! Class for Resource Acquisition Is Initialization (RAII) of Curl.
    class CurlPtr
    {
      public:

       //! Constructor.
       CurlPtr()
       {
         curl_ = curl_easy_init();
       }

       CurlPtr(CurlPtr&& curl) = default;
       CurlPtr& operator=(CurlPtr&& other) = default;

       CurlPtr(CurlPtr& curl) = delete;
       CurlPtr& operator=(const CurlPtr& other) = delete;


       /*!
           The init function performs the function curl_easy_cleanup closing all handle connections
           curl and then performs the initialization of the curl.
       */
       void init()
       {
         curl_easy_cleanup(curl_);
         curl_ = curl_easy_init();
       }

       //! Assume ownership of curl.
       CURL* fcurl() const
       {
         return curl_;
       }

       virtual CURLcode verifyURL(std::string url)
       {
         curl_easy_setopt(curl_, CURLOPT_URL, url.c_str());
         curl_easy_setopt(curl_, CURLOPT_FTPLISTONLY, 1);
         curl_easy_setopt(curl_, CURLOPT_CONNECTTIMEOUT, 3);
         curl_easy_setopt(curl_, CURLOPT_NOBODY, 1);

         return curl_easy_perform(curl_);
       }

       /*! When CurlPtr destructor is called, the function curl_easy_cleanup is used automatically.
           The function curl_easy_cleanup close all connections this handle curl.
       */
       virtual ~CurlPtr()
       {
         curl_easy_cleanup(curl_);
       }

      private:
        CURL* curl_; //!< Attribute for Handler Curl.
     };

  }
}

#endif //__TERRAMA2_CORE_UTILITY_CURLPTR_HPP__
