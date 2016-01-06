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
  \file terrama2/collector/CurlOpener.hpp

  \brief Curl Opener.

 \author Evandro Delatin
*/


#ifndef __TERRAMA2_COLLECTOR_CURLOPENER_HPP__
#define __TERRAMA2_COLLECTOR_CURLOPENER_HPP__


// Libcurl
#include <curl/curl.h>


namespace terrama2
{
  namespace collector
  {
    /*!
         * \brief The CurlOpener class implements the RAII technique to control operations with Curl.
         *
    */

    class CurlOpener
    {
      public:

//! Constructor.
       CurlOpener();

//! The init function performs the function curl_easy_cleanup closing all handle connections curl and then performs the initialization of the curl.
       void init();

//! Assume ownership of curl.
       CURL* fcurl() const;

//! When CurlOpener destructor is called, the function curl_easy_cleanup is used automatically. The function curl_easy_cleanup close all connections this handle curl.
       ~CurlOpener();

      private:
        CURL* curl_; //!< Attribute for Handler Curl.
     };
  }
}
    
#endif //__TERRAMA2_COLLECTOR_CURLOPENER_HPP__
