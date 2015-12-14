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
#include "DataProvider.hpp"
#include "DataSet.hpp"
#include "DataSetItem.hpp"
#include "Filter.hpp"

// STL
#include <string>

class QJsonDocument;

namespace terrama2
{
  namespace core
  {

    /*!
      \brief Returns the path relative to a directory or file in the context of TerraMA2.

      \param p A path to be searched in the TerraMA2 context.

      \return A complete path to the file or directory if it is found, otherwise returns an empty string.
     */
    std::string FindInTerraMA2Path(const std::string& p);

    /*!
      \brief Read a JSON document and parse it.

      \param file_name The file to be read (with absolute or relative path).

      \return A valid JSON document or throws an exception.

      \exception FileOpenError Throw this type of exception if a file could not be open.
      \exception ParserError  Throw this type of exception if JSON document is not valid.
     */
    QJsonDocument ReadJsonFile(const std::string& fileName);

    /*!
      \brief Returns a boolean with the status of the server based on the given status.

      \param status Enum with the status of the data provider.

      \return Boolean that determine if the data provider is active.
     */
    bool ToBool(DataProvider::Status status);

    /*!
      \brief Returns a enum with the status of the server based on the given parameter.

      \param active Boolean that determine if the data provider is active.

      \return Enum with the status of the data provider.
     */
    DataProvider::Status ToDataProviderStatus(bool active);

    /*!
      \brief Returns a enum with the kind of the server based on the given parameter.

      \param kind Kind that determine the type of the data provider.

      \return Enum with the type of the data provider.
     */
    DataProvider::Kind ToDataProviderKind(uint64_t kind);

    /*!
      \brief Function to convert a boolean to string format.

      \param b Boolean to converted.

      \return string String representation of the given boolean.
     */
    std::string ToString(bool b);

    /*!
      \brief Returns a boolean with the status of the dataset based on the given status.

      \param status Enum with the status of the dataset.

      \return Boolean that determine if the dataset is active.
     */
    bool ToBool(DataSet::Status status);

    /*!
      \brief Returns a enum with the status of the dataset based on the given parameter.

      \param active Boolean that determine if the dataset is active.

      \return Enum with the status of the dataset.
     */
    DataSet::Status ToDataSetStatus(bool active);

    /*!
      \brief Returns a enum with the kind of the dataset based on the given parameter.

      \param kind Kind that determine the type of the dataset.

      \return Enum with the type of the dataset.
     */
    DataSet::Kind ToDataSetKind(uint64_t kind);

    /*!
      \brief Returns a boolean with the status of the dataset item based on the given status.

      \param status Enum with the status of the dataset item.

      \return Boolean that determine if the dataset item is active.
     */
    bool ToBool(DataSetItem::Status status);

    /*!
      \brief Returns a enum with the status of the dataset item based on the given parameter.

      \param active Boolean that determine if the dataset item is active.

      \return Enum with the status of the dataset item.
     */
    DataSetItem::Status ToDataSetItemStatus(bool active);

    /*!
      \brief Returns a enum with the kind of the dataset item based on the given parameter.

      \param kind Kind that determine the type of the dataset item.

      \return Enum with the type of the dataset item.
     */
    DataSetItem::Kind ToDataSetItemKind(uint64_t kind);

    /*!
      \brief Returns a enum with the type of filter based on the given parameter.

      \param kind Kind that determine the type of filter.

      \return Enum with the type of filter.
     */
    Filter::ExpressionType ToFilterExpressionType(uint64_t type);

    /* \brief Initializes the terralib and load the plugins. */
    void initializeTerralib();

    /* \brief Finalizes the terralib. */
    void finalizeTerralib();


  } // end namespace core
}   // end namespace terrama2

#endif // __TERRAMA2_CORE_UTILS_HPP__
