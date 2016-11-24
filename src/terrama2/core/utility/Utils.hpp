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
  \file terrama2/core/Utils.hpp

  \brief Utility functions for TerraMA2 SDK.

  \author Gilberto Ribeiro de Queiroz
*/

#ifndef __TERRAMA2_CORE_UTILS_HPP__
#define __TERRAMA2_CORE_UTILS_HPP__

// TerraMA2
#include "../data-model/DataProvider.hpp"
#include "../data-model/DataSeries.hpp"
#include "../data-model/DataSet.hpp"
#include "../data-model/Filter.hpp"

// STL
#include <string>

#include <terralib/geometry/Coord2D.h>

// Forward declaration
class QJsonDocument;

// Forward declaration
namespace te
{
  namespace dt
  {
    class TimeInstant;
  }
}

// Forward declaration
namespace te
{
  namespace gm
  {
    class Geometry;
  }
}

namespace terrama2
{
  namespace core
  {
    /*!
      \brief Returns the path relative to a directory or file in the context of TerraMA2.

      \param p A path to be searched in the TerraMA2 context.

      \return A complete path to the file or directory if it is found, otherwise returns an empty string.
     */
    std::string FindInTerraMA2Path(const std::string& fileName);

    /*! \brief Initializes the terralib and load the plugins. */
    void initializeTerralib();

    /*! \brief Finalizes the terralib. */
    void finalizeTerralib();

    /*!
      \brief Initializes terrama2 logger module.
      \param pathFile A string value defining where is to open/save log file.
    */
    void initializeLogger(const std::string& pathFile);

    /*!
      \brief It disables the global TerraMA2 logger module.

      It may be useful during tests execution to do not display status message in data stream.
    */
    void disableLogger();

    /*!
      \brief It enables the global TerraMA2 logger module.

      \note It just enable logging core application. It is not guaranteed that TerraMA2 logger has been initialized to display output in console and file.
    */
    void enableLogger();

    /*!
      \brief Returns the SRID of a UTM projection based on the zone of given coordinate.
    */
    int getUTMSrid(te::gm::Geometry* geom);

    /*!
      \brief Converts the distance from the given unit to the target unit.
    */
    double convertDistanceUnit(double distance, const std::string& fromUnit, const std::string& targetUnit);

    DataSeriesType dataSeriesTypeFromString(const std::string& type);

    DataSeriesTemporality dataSeriesTemporalityFromString(const std::string& temporality);

    bool isValidColumn(size_t value);

    std::string getProperty(DataSetPtr dataSet, DataSeriesPtr dataSeries, std::string tag, bool logErrors = true);

    std::shared_ptr<te::gm::Geometry> ewktToGeom(const std::string& ewkt);

    /*!
      \brief Remove complex characters from the string

      This method remove any non-alphanumeric character:
      - digits (0123456789)
      - uppercase letters (ABCDEFGHIJKLMNOPQRSTUVWXYZ)
      - lowercase letters (abcdefghijklmnopqrstuvwxyz)

      Spaces are replaced by "_".
    */
    std::string simplifyString(std::string text);

    te::gm::Coord2D GetCentroidCoord(te::gm::Geometry* geom);

    /*!
      \brief Returns the value for the "is_temporal" property of the given dataset.
    */
    bool isTemporal(terrama2::core::DataSetPtr dataset);

    /*!
      \brief Returns the value for the time interval unit property of the given dataset.
    */
    std::string getTimeIntervalUnit(terrama2::core::DataSetPtr dataset);

    /*!
      \brief Returns the value for the time interval property of the given dataset.
    */
    double getTimeInterval(terrama2::core::DataSetPtr dataset);


    /*!
      \brief Returns the value for the folder property of the given dataset.
    */
    std::string getFolderMask(DataSetPtr dataSet, DataSeriesPtr dataSeries);

  } // end namespace core
}   // end namespace terrama2


namespace std
{
    template<> struct hash<terrama2::core::Filter>
    {
        size_t operator()(terrama2::core::Filter const& filter) const;
    };
}

#endif // __TERRAMA2_CORE_UTILS_HPP__
