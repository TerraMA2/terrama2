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
  \file terrama2/services/collector/core/IntersectionOperation.hpp

  \brief Instersects the collected data with the intersection data configured in the dataset,
  in order to add more information to the collected data.

  \author Paulo R. M. Oliveira
*/


#ifndef __TERRAMA2_SERVICES_COLLECTOR_CORE_INTERSECTIONOPERATION_HPP__
#define __TERRAMA2_SERVICES_COLLECTOR_CORE_INTERSECTIONOPERATION_HPP__

#include "Intersection.hpp"
#include "../core/Shared.hpp"
#include "../../../core/data-access/DataSetSeries.hpp"

// STL
#include <memory>
#include <string>
#include <vector>

namespace te
{
  // Forward declaration
  namespace da
  {
    class DataSet;

    class DataSetType;
  }

  namespace dt
  {
    class Property;
  }
}

namespace terrama2
{
  namespace services
  {
    namespace collector
    {
      namespace core
      {
        /*!
         \brief Intersects the collected data with the intersection data configured in the dataset, in order to add
         more information to the collected data.

         \param dataManager Smart pointer to the data manager.
         \param intersection Intersection configuration for the collected DataSeries.
         \param collectedDataSetSeries DataSetSeries of the collected data.
         \return The new DataSetSeries with added columns from intersection.
         */
        terrama2::core::DataSetSeries processIntersection(DataManagerPtr dataManager, IntersectionPtr intersection,
                                                          terrama2::core::DataSetSeries collectedDataSetSeries);

        /*!
         \brief Creates a DataSetType with the attributes from the collected dataset plus the attributes configured in the intersection.

         \param collectedDST DataSetType of the collected DataSet.
         \param intersectionDST DataSetType of the intersection DataSet.
         \param intersectionDSProperties Vector of properties to be added to the collected DataSet.
         \return A DataSetType with the attributes from the collected dataset plus the attributes configured in the intersection.
         */
        te::da::DataSetType* createDataSetType(te::da::DataSetType* collectedDST,
                                               te::da::DataSetType* intersectionDST,
                                               std::vector<te::dt::Property*> intersectionDSProperties);

        /*!
         \brief Process the intersection for vector data.

         \param dataManager Smart pointer to the data manager.
         \param intersection Intersection configuration for the collected DataSeries.
         \param collectedDataSetSeries DataSetSeries of the collected data.
         \paran vecAttributes List of attributes to be added to the collected DataSeries.
         \param intersectionDataSeries DataSet to be used in the intersection.
         \return The new DataSetSeries with added columns from intersection.
         */
        terrama2::core::DataSetSeries processVectorIntersection(DataManagerPtr dataManager,
                                                                  core::IntersectionPtr intersection,
                                                                  terrama2::core::DataSetSeries collectedDataSetSeries,
                                                                  std::vector<std::string>& vecAttributes,
                                                                  terrama2::core::DataSeriesPtr intersectionDataSeries);

        /*!
         \brief Process the intersection for grid data.

         \param dataManager Smart pointer to the data manager.
         \param intersection Intersection configuration for the collected DataSeries.
         \param collectedDataSetSeries DataSetSeries of the collected data.
         \param intersectionDataSeries DataSet to be used in the intersection.
         \return The new DataSetSeries with added columns from intersection.
         */
        terrama2::core::DataSetSeries processGridIntersection(DataManagerPtr dataManager,
                                                                  core::IntersectionPtr intersection,
                                                                  terrama2::core::DataSetSeries collectedDataSetSeries,
                                                                  std::vector<std::string> vecAttr,
                                                                  terrama2::core::DataSeriesPtr intersectionDataSeries);

        /*!
          \brief Get list of bands to be intersected.
        */
        std::vector<int> getBands(std::vector<std::string> vecAttr);
      } // end namespace core
    }   // end namespace collector
  }     // end namespace services
}       // end namespace terrama2

#endif //__TERRAMA2_SERVICES_COLLECTOR_CORE_INTERSECTIONOPERATION_HPP__
