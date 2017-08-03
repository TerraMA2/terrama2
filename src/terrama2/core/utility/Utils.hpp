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

// TerraLib
#include <terralib/geometry/Coord2D.h>
#include <terralib/raster/Raster.h>
#include <terralib/dataaccess/dataset/DataSet.h>

// STL
#include <string>


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

    DataSeriesType dataSeriesTypeFromString(const std::string& type);

    DataSeriesTemporality dataSeriesTemporalityFromString(const std::string& temporality);

    bool isValidColumn(size_t value);

    std::string getProperty(DataSetPtr dataSet, DataSeriesPtr dataSeries, std::string tag, bool logErrors = true);

    /*!
      \brief Remove complex characters from the string

      This method remove any non-alphanumeric character:
      - digits (0123456789)
      - uppercase letters (ABCDEFGHIJKLMNOPQRSTUVWXYZ)
      - lowercase letters (abcdefghijklmnopqrstuvwxyz)

      Spaces are replaced by "_".
    */
    std::string simplifyString(std::string text);

    /*!
      \brief Returns the value for the "is_temporal" property of the given dataset.
    */
    bool isTemporal(terrama2::core::DataSetPtr dataset);

    /*!
      \brief Returns the value for the time interval property of a temporal dataset.
    */
    std::string getTimeInterval(terrama2::core::DataSetPtr dataset);


    size_t propertyPosition(const te::da::DataSet* dataSet, const std::string& propertyName);

    /*!
     * \brief Returns a valid name for te::da::DataSet properties name
     * \param text The base name to process
     * \return A valid name for te::da::DataSet properties name
     */
    std::string createValidPropertyName(const std::string& oldName);

    /*!
     * \brief Split a string based in a delimiter and returns the partis in a vector.
     * This method does not skip empty parts.
     * \param text The strint to be splitted
     * \param delim The delimiter character
     * \return A vector with the splitted parts of the text.
     */
    std::vector<std::string> splitString(const std::string& text, char delim);

    /*!
     * \brief Get all the dates avaible from a dataset column, the returned vector do not has duplicated dates and
     * is ASC ordered
     * \param teDataset
     * \param datetimeColumnName
     * \return A vector with dates, with no duplicated dates
     */
    std::vector<std::shared_ptr<te::dt::DateTime> > getAllDates(te::da::DataSet* teDataset,
                                                                const std::string& datetimeColumnName);

    //! Recover complete mask, folder plus file mask
    std::string getMask(DataSetPtr dataset);

    //! Recover table name of the DCP postions table_name
    std::string getDCPPositionsTableName(DataSeriesPtr dataSeries);

    //Returns the file mask .
    std::string getFileMask(DataSetPtr dataSet);

    //Returns the folder mask .
    std::string getFolderMask(DataSetPtr dataSet);

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
