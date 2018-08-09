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

#include <terralib/dataaccess/dataset/DataSet.h>
#include <terralib/dataaccess/dataset/DataSetType.h>
#include <terralib/core/Config.h>
#include <terralib/dataaccess/datasource/DataSource.h>
// TerraLib
#include <terralib/geometry/Coord2D.h>
#include <terralib/raster/Raster.h>
#include <memory>
// STL
#include <string>
#include <utility>
#include <vector>

#include "../Shared.hpp"
#include "../Typedef.hpp"
// TerraMA2
#include "../data-model/DataProvider.hpp"
#include "../data-model/DataSeries.hpp"
#include "../data-model/DataSeriesSemantics.hpp"
#include "../data-model/DataSet.hpp"
#include "../data-model/Filter.hpp"

// Forward declaration
class QJsonDocument;
namespace terrama2 {
namespace core {
struct Filter;
}  // namespace core
}  // namespace terrama2
namespace te {
namespace da {
class DataSet;
class DataSetType;
class DataSource;
}  // namespace da
namespace dt {
class TimeInstantTZ;
}  // namespace dt
}  // namespace te

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
    class FileRemover;
    
    /*!
      \brief Returns the path relative to a directory or file in the context of TerraMA2.

      \param p A path to be searched in the TerraMA2 context.

      \return A complete path to the file or directory if it is found, otherwise returns an empty string.
     */
    TECOREEXPORT std::string FindInTerraMA2Path(const std::string& fileName);

    /*! \brief Initializes the terralib and load the plugins. */
    TECOREEXPORT void initializeTerralib();

    /*! \brief Finalizes the terralib. */
    TECOREEXPORT void finalizeTerralib();

    /*!
      \brief Initializes terrama2 logger module.
      \param pathFile A string value defining where is to open/save log file.
    */
    TECOREEXPORT void initializeLogger(const std::string& pathFile);

    /*!
      \brief It disables the global TerraMA2 logger module.

      It may be useful during tests execution to do not display status message in data stream.
    */
    TECOREEXPORT void disableLogger();

    /*!
      \brief It enables the global TerraMA2 logger module.

      \note It just enable logging core application. It is not guaranteed that TerraMA2 logger has been initialized to display output in console and file.
    */
    TECOREEXPORT void enableLogger();

    TECOREEXPORT DataSeriesType dataSeriesTypeFromString(const std::string& type);

    TECOREEXPORT DataSeriesTemporality dataSeriesTemporalityFromString(const std::string& temporality);

    TECOREEXPORT bool isValidColumn(size_t value);

    TECOREEXPORT std::string getProperty(DataSetPtr dataSet, DataSeriesPtr dataSeries, std::string tag, bool logErrors = true);

    /*!
      \brief Remove complex characters from the string

      This method remove any non-alphanumeric character:
      - digits (0123456789)
      - uppercase letters (ABCDEFGHIJKLMNOPQRSTUVWXYZ)
      - lowercase letters (abcdefghijklmnopqrstuvwxyz)

      Spaces are replaced by "_".
    */
    TECOREEXPORT std::string simplifyString(std::string text);

    TECOREEXPORT std::string readFileContents(const std::string& absoluteFilePath);

    /*!
      \brief Returns the value for the "is_temporal" property of the given dataset.
    */
    TECOREEXPORT bool isTemporal(terrama2::core::DataSetPtr dataset);

    /*!
      \brief Returns the value for the time interval property of a temporal dataset.
    */
    TECOREEXPORT std::string getTimeInterval(terrama2::core::DataSetPtr dataset);


    TECOREEXPORT size_t propertyPosition(const te::da::DataSet* dataSet, const std::string& propertyName);

    /*!
     * \brief Returns a valid name for te::da::DataSet properties name
     * \param text The base name to process
     * \return A valid name for te::da::DataSet properties name
     */
    TECOREEXPORT std::string createValidPropertyName(const std::string& oldName);

    /*!
     * \brief Split a string based in a delimiter and returns the partis in a vector.
     * This method does not skip empty parts.
     * \param text The strint to be splitted
     * \param delim The delimiter character
     * \return A vector with the splitted parts of the text.
     */
    TECOREEXPORT std::vector<std::string> splitString(const std::string& text, char delim);

    /*!
     * \brief Get all the dates avaible from a dataset column, the returned vector do not has duplicated dates and
     * is ASC ordered
     * \param teDataset
     * \param datetimeColumnName
     * \return A vector with dates, with no duplicated dates
     */

    TECOREEXPORT std::vector<std::shared_ptr<te::dt::TimeInstantTZ> > getAllDates(te::da::DataSet* teDataset,
                                                                const std::string& datetimeColumnName);

    //! Recover complete mask, folder plus file mask
    TECOREEXPORT std::string getMask(DataSetPtr dataset);

    //! Recover table name of the DCP postions table_name
    TECOREEXPORT std::string getDCPPositionsTableName(DataSeriesPtr dataSeries);

    //! Returns the file mask .
    TECOREEXPORT std::string getFileMask(DataSetPtr dataSet);

    //! Returns the folder mask .
    TECOREEXPORT std::string getFolderMask(DataSetPtr dataSet);

    //! Return the te::da::DataSetType and te::da::DataSet of the DCP positions table
    TECOREEXPORT std::pair<std::shared_ptr<te::da::DataSetType>, std::shared_ptr<te::da::DataSet> >
    getDCPPositionsTable(std::shared_ptr<te::da::DataSource> datasource, const std::string& dataSetName);

      
    /**
     * @brief Get a new Temporary Folder.
     * 
     * @param oldTempTerraMAFolder Optional temporary folder to be used.
     * @return std::string Path of the temporary folder.
     */
    TECOREEXPORT std::string
    getTemporaryFolder( std::shared_ptr<terrama2::core::FileRemover> remover,
                        const std::string& oldTempTerraMAFolder = "");
    /*!
      \brief Erase content from previous execution

      This method will erase from a postgres table the results with date: startTime
      It is used for reprocessing historical data.
    */
    TECOREEXPORT void erasePreviousResult(DataManagerPtr dataManager, DataSeriesId dataSeriesId, std::shared_ptr<te::dt::TimeInstantTZ> startTime);
  } // end namespace core
}   // end namespace terrama2


namespace std
{
    template<> struct TECOREEXPORT hash<terrama2::core::Filter>
    {
        size_t operator()(terrama2::core::Filter const& filter) const;
    };
}

#endif // __TERRAMA2_CORE_UTILS_HPP__
