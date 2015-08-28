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
  \file terrama2/ws/collector/core/Tiff.hpp

  \brief Implementation of a collector for the Tiff format.

  \author Paulo R. M. Oliveira
*/

#ifndef __TERRAMA2_WS_COLLECTOR_CORE_TIFF_HPP__
#define __TERRAMA2_WS_COLLECTOR_CORE_TIFF_HPP__

#include "Collector.hpp"


namespace terrama2
{
  namespace ws
  {
    namespace collector
    {
      namespace core
      {

        /**
         * \brief Class that contains the implementation of a collector for the Tiff format.
         *
         * Dynamic metadata for a Tiff format
         * navigation_file: string
         */
        class Tiff : public Collector
        {
        public:
          /**
           * \brief Constructor
           */
          Tiff();

          /**
           * \brief Constructor
           */
          Tiff(const int& id, const std::string& name, const std::string& description, const std::string& type, const Format& format, const int& updateFreqMinutes, const std::string& prefix, const int& srid, const std::string& mask, const std::string& unit, const std::string& timeZone, const QJsonObject& dynamicMetadata);

          /**
          * \brief Destructor
          */
          virtual ~Tiff();

          /*!
            \brief This method should be implemented by each subclass
            in order to read the collected file and store it in a format that can be used in terralib.
          */
          void collect(const std::string& file);

        };      
      } // core    
    } // collector
  } // ws
} // terrama2

#endif // __TERRAMA2_WS_COLLECTOR_CORE_TIFF_HPP__
