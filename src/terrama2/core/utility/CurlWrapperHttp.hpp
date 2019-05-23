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
  \file terrama2/core/utility/CurlWrapperHttp.hpp
  \brief Utility classes for CurlWrapperHttp.

  \author Jean Souza
*/

#ifndef __TERRAMA2_CORE_UTILITY_CURL_WRAPPER_HTTP_HPP__
#define __TERRAMA2_CORE_UTILITY_CURL_WRAPPER_HTTP_HPP__

// TerraMa2
#include "../Config.hpp"

#include "CurlWrapperFtp.hpp"

// TerraLib
#include <terralib/ws/core/CurlWrapper.h>

namespace terrama2
{
  namespace core
  {
    /**
     * @brief RAII class for accessing HTTP servers
     * 
     */
    class TMCOREEXPORT CurlWrapperHttp : public CurlWrapperFtp
    {
      public:
        using te::ws::core::CurlWrapper::downloadFile;
        using te::ws::core::CurlWrapper::verifyURL;

        /**
         * @brief Method to download a file retrieved from given URL and save into the specified File.
         * 
         * @note This is a wrapper method to set some needed flags in the curl object.
         */
        virtual void downloadFile(const std::string &url, std::FILE* file, te::common::TaskProgress* taskProgress = nullptr) override;
        virtual std::vector<std::string> listFiles(const te::core::URI& uri) override;
    };
  } /* core */
} /* terrama2 */

#endif // __TERRAMA2_CORE_UTILITY_CURL_WRAPPER_HTTP_HPP__
