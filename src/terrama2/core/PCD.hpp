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
  \file terrama2/core/PCD.hpp

  \brief Models the information of a PCD, it's a wrapper to extract data from the DataSetItem's metadata.

  \author Paulo R. M. Oliveira
*/

#ifndef __TERRAMA2_CORE_PCD_HPP__
#define __TERRAMA2_CORE_PCD_HPP__

// TerraMA2
#include "DataSetItem.hpp"

// STL
#include <string>

// Forward declaration
namespace te
{
  namespace gm
  {
    class Point;
  }
}

namespace terrama2
{
  namespace core
  {

    /*!
      \class PCD

      \brief Models the information about a PCD, it's a wrapper to extract data from the DataSetItem's metadata.

     */
    class PCD
    {
      public:

        const std::string UNIT = "UNIT"; //!< DataSetItem metadata key for unit of measurement.
        const std::string PREFIX = "PREFIX"; //!< DataSetItem metadata key for a PCD prefix.
        const std::string LATITUDE = "LATITUDE"; //!< DataSetItem metadata key for the latitude of a PCD.
        const std::string LONGITUDE = "LONGITUDE"; //!< DataSetItem metadata key for the longitude of a PCD.

        /*! \brief Constructor. */
        PCD(DataSetItem& dataSetItem);

        /*! \brief Returns the unit of measurement from the DataSetItem metadata. */
        std::string unit() const;

        /*! \brief Sets the unit of measurement in the DataSetItem metadata. */
        void setUnit(const std::string unit);

        /*! \brief Returns the prefix used to collect the data from the DataSetItem metadata. */
        std::string prefix() const;

        /*! \brief Sets the prefix used to collect the data in the DataSetItem metadata. */
        void setPrefix(const std::string prefix);

        /*! \brief Creates a point with the latitude and longitude information from the DataSetItem metadata. */
        te::gm::Point* location() const;

        /*! \brief Sets the location of a PCD. */
        void setLocation(te::gm::Point* location);

        /*! \brief Returns the DataSetItem object. */
        DataSetItem dataSetItem() const;

        /*! \brief Overloads the operator to create a implici cast to DataSetItem. */
        operator DataSetItem&();

      private:
        DataSetItem dataSetItem_; //!< The DataSetItem that represents this PCD.

    };

  } // end namespace core
}   // end namespace terrama2

#endif  // __TERRAMA2_CORE_PCD_HPP__

