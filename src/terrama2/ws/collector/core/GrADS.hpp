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
  \file terrama2/ws/collector/core/GrADS.hpp

  \brief Implementation of a collector for the GrADS format.

  \author Paulo R. M. Oliveira
*/

#ifndef __TERRAMA2_WS_COLLECTOR_CORE_GRADS_HPP__
#define __TERRAMA2_WS_COLLECTOR_CORE_GRADS_HPP__

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
         * @brief The GrADS class
         *
         * Dynamic metadata for a GrADS format
         * controlFile:string
         * isFloat: bool
         * multiplier: double
         * isBigendian: bool
         * bandQuantity: int
         * bandHeader: int
         * bandTrailler: int
         * bandTimeOffset: int
         */
        class GrADS : public Collector
        {
        public:
          /**
           * \brief Constructor
           */
          GrADS();

          /**
          * \brief Destructor
          */
          virtual ~GrADS();

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

#endif // __TERRAMA2_WS_COLLECTOR_CORE_GRADS_HPP__
