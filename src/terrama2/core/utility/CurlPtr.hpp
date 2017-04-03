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
#include <iostream>
#include <functional>
#include <vector>

// Boost
#include <boost/algorithm/string.hpp>

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
        CurlPtr();

        CurlPtr(CurlPtr&& other);

        CurlPtr& operator=(CurlPtr&& other);

        CurlPtr(CurlPtr& curl) = delete;
        CurlPtr& operator=(const CurlPtr& other) = delete;

        /*!
           The init function performs the function curl_easy_cleanup closing all handle connections
           curl and then performs the initialization of the curl.
       */
        void init();

        //! Assume ownership of curl.
        CURL* fcurl() const;

        //! The function verifyURL checks if the "url" parameter is passed by valid.
        virtual CURLcode verifyURL(std::string url, uint32_t timeout);

        //! The function getListFiles returns vector with the files found on the server.
        virtual std::vector<std::string> getFtpList(const std::string& url,
                                                      size_t(*write_vector)(void *ptr, size_t size, size_t nmemb, void *data)) const;

        virtual std::vector<std::string> getFtpListDir(const std::string& url,
                                                       size_t(*write_vector)(void *ptr, size_t size, size_t nmemb, void *data)) const;

        virtual std::vector<std::string> getFtpListFiles(const std::string& url,
                                                         size_t(*write_vector)(void *ptr, size_t size, size_t nmemb, void *data)) const;

        //! The function getDownloadFiles performs download the filtered files and returns it succeded or not.
        virtual CURLcode getDownloadFiles(std::string url,
                                          size_t(*write_response)(void *ptr, size_t size, size_t nmemb, void *data),
                                          std::string filePath);

        /*! When CurlPtr destructor is called, the function curl_easy_cleanup is used automatically.
           The function curl_easy_cleanup close all connections this handle curl.
       */
        virtual ~CurlPtr();

      private:
        CURL* curl_; //!< Attribute for Handler Curl.
    };

  }
}

#endif //__TERRAMA2_CORE_UTILITY_CURLPTR_HPP__
