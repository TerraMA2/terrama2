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
  \file terrama2/services/collector/core/Intersection.hpp

  \brief Intersection information of a DataSet.

  \author Paulo R. M. Oliveira
*/

#ifndef __TERRAMA2_SERVICES_COLLECTOR_CORE_INTERSECTION_HPP__
#define __TERRAMA2_SERVICES_COLLECTOR_CORE_INTERSECTION_HPP__

#include "../core/Typedef.hpp"

// STL
#include <vector>
#include <map>
#include <string>

namespace terrama2
{
  namespace services
  {
    namespace collector
    {
      namespace core
      {

        struct IntersectionAttribute
        {
          std::string attribute;
          std::string alias;
        };

        /*!
          \class Intersection

          \brief Intersection information of a DataSeries.
         */
        struct Intersection
        {
          CollectorId collectorId; //!< Collector identifier.
          std::map<DataSeriesId, std::vector<IntersectionAttribute> > attributeMap; //!< Map with the attributes of a DataSeries to be added to the collected data.
        };

      } // end namespace core
    }   // end namespace collector
  }     // end namespace services
}       // end namespace terrama2

#endif  // __TERRAMA2_SERVICES_COLLECTOR_CORE_INTERSECTION_HPP__
