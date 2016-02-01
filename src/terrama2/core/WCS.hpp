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
  \file terrama2/core/WCS.hpp

  \brief Models the information of a WCS, it's a wrapper to extract data from the DataSetItem's metadata.

  \author Vinicius Campanha
*/

#ifndef __TERRAMA2_CORE_WCS_HPP__
#define __TERRAMA2_CORE_WCS_HPP__

// TerraMA2
#include "DataSetItem.hpp"

// STL
#include <string>

namespace terrama2
{
  namespace core
  {

    /*!
      \class WCS

      \brief Models the information about a WCS, it's a wrapper to extract data from the DataSetItem's metadata.

     */
    class WCS
    {
      public:

        const std::string REQUEST = "REQUEST"; //!< DataSetItem metadata key for request in WCS
        const std::string VERSION = "VERSION"; //!< DataSetItem metadata key for the version of WCS service
        const std::string COVERAGEID = "COVERAGEID"; //!< DataSetItem metadata key for coverage identification
        const std::string FORMAT = "FORMAT"; //!< DataSetItem metadata key for data format request
        const std::string SCALEFACTOR = "SCALEFACTOR"; //!< DataSetItem metadata key for the scale factor
        const std::string SUBSET = "SUBSET"; //!< DataSetItem metadata key for the subsets

        /*! \brief Constructor. */
        WCS(DataSetItem& dataSetItem);

        /*! \brief Returns a value of the passed parameter from the DataSetItem metadata */
        std::string getParameter(std::string parameter);

        /*! \brief Set in DataSetItem metadata the key and value given */
        void setParameter(std::string parameterKey, std::string parameterValue);

        /*! \brief Returns the request value from the DataSetItem metadata. */
        std::string request() const;

        /*! \brief Sets the request value in the DataSetItem metadata. */
        void setRequest(const std::string request);

        /*! \brief Returns the version of WCS service from the DataSetItem metadata. */
        std::string version() const;

        /*! \brief Sets the version of WCS service in the DataSetItem metadata. */
        void setVersion(const std::string version);

        /*! \brief Returns the coverage ID from DataSetItem metadata*/
        std::string coverageId() const;

        /*! \brief Sets the coverage ID in the DataSetItem metadata */
        void setCoverageId(const std::string coverageId);

        /*! \brief Returns the format from DataSetItem metadata*/
        std::string format() const;

        /*! \brief Sets the format in the DataSetItem metadata */
        void setFormat(const std::string format);

        /*! \brief Returns the scale factor from DataSetItem metadata */
        std::string scaleFactor() const;

        /*! \brief Sets the scale factor in the DataSetItem metadata */
        void setScaleFactor(const std::string scaleFactor);

        /*! \brief Returns the subset from DataSetItem metadata */
        std::string subset() const;

        /*! \brief Sets the subset in the DataSetItem metadata */
        void setSubset(const std::string subset);

        /*! \brief Returns the DataSetItem object. */
        DataSetItem dataSetItem() const;

        /*! \brief Overloads the operator to create a implici cast to DataSetItem. */
        operator DataSetItem&();

      private:
        DataSetItem dataSetItem_; //!< The DataSetItem that represents this WCS.

    };

  } // end namespace core
}   // end namespace terrama2

#endif  // __TERRAMA2_CORE_WCS_HPP__

