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

#include "DataProvider.hpp"
#include "DataSet.hpp"
#include "Data.hpp"

// STL
#include <string>

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
      \brief Returns a boolean with the status of the server based on the given status.

      \param status Enum with the status of the data provider.

      \return Boolean that determine if the data provider is active.
     */
    bool DataProviderStatusToBool(terrama2::core::DataProvider::Status status);

    /*!
      \brief Returns a enum with the status of the server based on the given parameter.

      \param active Boolean that determine if the data provider is active.

      \return Enum with the status of the data provider.
     */
   terrama2::core::DataProvider::Status BoolToDataProviderStatus(bool active);

   /*!
     \brief Returns a enum with the kind of the server based on the given parameter.

     \param kind Kind that determine the type of the data provider.

     \return Enum with the type of the data provider.
    */
   terrama2::core::DataProvider::Kind IntToDataProviderKind(uint64_t kind);

   /*!
     \brief Function to convert a boolean to string format.

     \param b Boolean to converted.

     \return string String representation of the given boolean.
    */
   std::string BoolToString(bool b);

   /*!
     \brief Returns a boolean with the status of the dataset based on the given status.

     \param status Enum with the status of the dataset.

     \return Boolean that determine if the dataset is active.
    */
   bool DataSetStatusToBool(terrama2::core::DataSet::Status status);

   /*!
    \brief Returns a enum with the status of the dataset based on the given parameter.

    \param active Boolean that determine if the dataset is active.

    \return Enum with the status of the dataset.
    */
   terrama2::core::DataSet::Status BoolToDataSetStatus(bool active);


   /*!
     \brief Returns a enum with the kind of the dataset based on the given parameter.

     \param kind Kind that determine the type of the dataset.

     \return Enum with the type of the dataset.
    */
   terrama2::core::DataSet::Kind IntToDataSetKind(uint64_t kind);

   /*!
     \brief Returns a boolean with the status of the data based on the given status.

     \param status Enum with the status of the data.

     \return Boolean that determine if the data is active.
    */
   bool DataStatusToBool(terrama2::core::Data::Status status);

   /*!
    \brief Returns a enum with the status of the data based on the given parameter.

    \param active Boolean that determine if the data is active.

    \return Enum with the status of the data.
    */
   terrama2::core::Data::Status BoolToDataStatus(bool active);

   /*!
     \brief Returns a enum with the kind of the data based on the given parameter.

     \param kind Kind that determine the type of the data.

     \return Enum with the type of the data.
    */
   terrama2::core::Data::Kind IntToDataKind(uint64_t kind);


  } // end namespace core
}   // end namespace terrama2

#endif // __TERRAMA2_CORE_UTILS_HPP__

